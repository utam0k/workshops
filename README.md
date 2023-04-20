# Rootlessコンテナ

## コンテナとは？

[slide](https://speakerdeck.com/utam0k/are-kontenatutehe-datuke-karasheng-mareta-rust-deshu-kareta-kontenarantaimu-youkifalsehua-at-odc2021?slide=10)

- cgroup
- namespaces <- 今回は主にここ
- pivot_root

### chroot(pivot_root)

```console
$ ROOTFS=$(mktemp -d)
$ echo "Hello" > $ROOTFS/test.txt
$ mkdir -p $ROOTFS/{bin,/lib/x86_64-linux-gnu,lib64}
$ cp -v /bin/{bash,cat} $ROOTFS/bin
$ ldd /bin/cat | grep -o '/lib.*\.[0-9]' | xargs -I{} cp -v {} $ROOTFS/{} 
$ ldd /bin/bash | grep -o '/lib.*\.[0-9]' | xargs -I{} cp -v {} $ROOTFS/{}
```

### Namespace

| 種類                               | 概要                               | 
| :--------------------------------: | :--------------------------------: | 
| User                               | uid/gidなど                        | 
| PID                                | process ids                        |
| Network                            | ip netns                           | 
| Cgroup                             | /sys/fs/cgroup                     | 
| UTC                                | hostname/domainnameなど            | 
| Time                               | 時間                               | 
| IPC(Inter-Process Communiation)    | System V IPC/POSIXメッセージキュー | 
| Mount                              | マウントポイント                   | 

```console
$ sudo docker export $(sudo docker create ubuntu) | tar -C $ROOTFS -xvf -　
$ sudo unshare --mount --uts --ipc --pid --fork --mount-proc --root rootfs sh
```


存在するコンテナに入る
```console
$ sudo docker run -d --rm --name sample-container alpine sleep 1d
$ PID=$(sudo docker inspect --format {{.State.Pid}} sample-container)
```

```console
$ sudo nsenter --target $PID --uts --mount --ipc --net --pid /bin/sh
/ # ps -ef
PID   USER     TIME  COMMAND
    1 root      0:00 sleep
   26 root      0:00 /bin/sh
   27 root      0:00 ps
/ # exit
```

## Rootlessコンテナ

Rootlessへの課題
- `CAP_SYS_ADMIN`
- uid/gid
- ファイルの所有権


## Rootlessコンテナ内でもrootユーザーになりたい！

###  User namespaces

CAP_SYS_ADMIN

コンテナ内では特権を持ちつつ、ホストOSでは特権を持たないユーザーを作る

- 見かけのUID = 0 -> いろいろな他のnamespaceが作れるようになる！ 
- user namespace外でアクセスできないファイルはやっぱりアクセスできない
- uidのマッピングはできる

`コンテナ内のuid:ホストでのuid:size`

```console
$ unshare --user
$ id
$ echo $$
[container pid]
```

another terminal

```console
$ cpid=$container_pid
$ grep "Uid" /proc/$cpid/status
$ echo '0 1000 1' > /proc/$cpid/uid_map
```

container terminal
```console
$ id
$ touch test.txt
```

host terminal
```console
$ ls -l test.txt
```

container terminal
```console
$ whoami
$ shutdown
$ unshare --net
$ ip a
```


TODO: podman with bpftrace
```console
$ podman run -it --rm --userns=auto:uidmapping=0:1000:1 --cap-add=sys_admin,mknod --device=/dev/fuse --security-opt label=disable quay.io/podman/stable echo hi
```

## Mount namespace

ほとんどのコンテナイメージでrootユーザーが作ったファイルがある...
chownする？
- 複数のユーザーの場合は？
- あまり効率的ではない

ほぼmountできない(overlayfsもマウントできない)

- fuse-overlayfs
- shiftfs
- id-mapped mount

https://gihyo.jp/article/2022/12/linux_containers-0050

```console
$ sudo docker export $(sudo docker create ubuntu) | sudo tar -C rootfs -xvf -
$ unshare --user --map-root-user --root rootfs sh
```

another
```console
$ sudo chown -R $(whoami):$(whoami) rootfs
```

### idmapped mount

```console
# Terminal A
$ mkdir rootfs; sudo docker export $(sudo docker create ubuntu) | sudo tar -C rootfs -xvf -
$ mkdir rootfs/test
$ sudo idmapped --map-mount b:0:1000:65536 $(pwd)/rootfs $(pwd)/mapped-rootfs
$ ls -l rootfs
$ ls -l mapped-rootfs
$ unshare --user --root mapped-rootfs bash
$ echo $$

# Terminal B
$ echo '0 1000 65536' | sudo tee /proc/$cid/uid_map
$ echo '0 1000 65536' | sudo tee /proc/$cid/gid_map

# Terminal A
$ cd test
$ touch test.txt
$ echo Hello > test.txt
```
