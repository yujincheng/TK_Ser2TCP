# TK_SER2TCP_串口转TCP模块

该项目完成于浙江爱迪曼（天控科技）暑期社会实践期间，主要目的是完成OpenWRT操作系统中从串口到TCP端口的透传。运行的目标芯片为 MT7628，是一款优秀的路由器芯片。

天控科技的流量计通过串口将要发送的命令给路由器芯片，路由器芯片将串口收到的数据发送给远端TCP server。

路由器芯片只起到一个流量转发的作用。

## 文件组织

在该项目中有三个文件

1. tcpclient.c

2. tcpclient.py

3. tcpserver.py

其中 tcpclient.c 为该项目主要程序。tcpserver.py 是服务器端执行的python3脚本，搭建一个简单的tcp环境用于测试。tcpclient.py 测试代码，验证TCP server是否工作。

## 使用方法

该文件在 Openwrt系统上运行，需要交叉编译,因為採用了多线程。交叉编译指令如下：

```shell
mipsel-openwrt-linux-gcc tcpclient.c -o testtcp.out -lpthread -ldl
```

生成的 testtcp.out 可执行文件可以直接放在 Openwrt 系统中执行。

## 实验注意事项
