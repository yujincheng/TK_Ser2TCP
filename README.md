爱迪曼公司已有的系统是购入现成的串口转网卡模块，利用现成模块和服务器建立TCP长连接（socket连接）。之后利用模块的透传模式，直接将串口和TCP socket进行连接。由于我们现在已经有了路由器模块，可以通过路由器串口和路由器的网络实现该功能。

# 文件结构

在OpenWRT 编译目录下，package文件夹中新建 目标 Ser2TCP 模块目录，该目录下文件结构如下：

```Shell
    [OpenWRT_Home]/package/Ser2TCP/Makefile   #编译整个项目的文件
                                  /src      /Makefile   #编译程序的Makefile
                                            /tcpclient.c    #程序源文件
                                  /files    /ser2TCP_cbi.lua    #lua界面配置
                                            /ser2TCP.config     #uci配置文件
                                            /ser2TCP_controller.lua #lua控制
                                            /ser2TCP.hotplug    #插件，在网口重启时实现该模块重启
                                            /ser2TCP.init   #/etc/init.d/ser2TCP
                                            /ser2TCP.uci    #lua关联文件
```

##  [OpenWRT_Home]/package/Ser2TCP/Makefile

该文件虽然叫 Makefile，但是并不是一个传统的Makefile。它的格式跟一般的Makefile不一样，因为它的功能跟普通Makefile就是不一样的，它是一种编写方便的模板。

本项目中文件如下：

```Makefile
#
# Copyright (C) 2008-2010 OpenWrt.org
# Copyright (C) 2016 fly.zhu <flyzjhz@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ser2TCP
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_MAINTAINER:=fly.zhu <flyzjhz@gmail.com>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/ser2TCP
  SECTION:=net
  CATEGORY:=Utilities
  DEPENDS:= +libpthread
  TITLE:=ser2TCP communication utility
endef


define Build/Prepare
        mkdir -p $(PKG_BUILD_DIR)
        $(CP) ./src/* $(PKG_BUILD_DIR)/
endef

# define Build/Compile
#       CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
#       $(MAKE) -C $(PKG_BUILD_DIR) \
#               $(TARGET_CONFIGURE_OPTS) \
#               LIBS="$(TARGET_LDFLAGS) -lm -lpthread"
# endef

define Package/ser2TCP/install
        $(INSTALL_DIR) $(1)/usr/sbin
        $(INSTALL_BIN) $(PKG_BUILD_DIR)/ser2TCP $(1)/usr/sbin/
        $(INSTALL_DIR) $(1)/etc/init.d
        $(INSTALL_BIN) ./files/ser2TCP.init $(1)/etc/init.d/ser2TCP
        $(INSTALL_DIR) $(1)/etc/config
        $(INSTALL_DATA) ./files/ser2TCP.config $(1)/etc/config/ser2TCP
        $(INSTALL_DIR) $(1)/etc/hotplug.d/iface
        $(INSTALL_DATA) ./files/ser2TCP.hotplug $(1)/etc/hotplug.d/iface/20-ser2TCP
        $(INSTALL_DIR) $(1)/usr/lib/lua/luci/controller
        $(INSTALL_DATA) ./files/ser2TCP_controller.lua $(1)/usr/lib/lua/luci/controller/ser2TCP.lua
        $(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi
        $(INSTALL_DATA) ./files/ser2TCP_cbi.lua $(1)/usr/lib/lua/luci/model/cbi/ser2TCP.lua
        $(INSTALL_DIR) $(1)/etc/uci-defaults
        $(INSTALL_BIN) ./files/ser2TCP.uci $(1)/etc/uci-defaults/luci-ser2TCP
endef

$(eval $(call BuildPackage,ser2TCP))
```

和我们构建工程相关，有如下几个步骤:

### 包含全局变量

首先在Makefile中的第一行一定要包含下面这个命令，这是Makefile的一些全局变量的相关定义
include $(TOPDIR)/rules.mk

### BuildPackage相关的宏定义

1. 描述软件包在menuconfig和ipkg中的信息，可以定义如下变量：

    ```Makefile
    define  Package/< PKG_NAME >
        SECTION : 软件包类型
        CATEGORY :  软件包在menuconfig里的位置，如Network, Utilities
        SUBMENU : menuconfig中软件包所属的二级目录，如dial-in/up
        DEFAULT: 默认的编译模式，m=编译成模块，y=编译到镜像，n或者不加不编译，[依赖
                包 两个之间通过空格分隔 前面加+为默认显示 选中该软件包自动选中依
                赖包 不加+为默认不显示 选中依赖包才显示] 
        TITLE : 软件包标题
        DESCRIPTION : 软件包的详细说明，由于存在bug，现在已经放弃
        URL : 软件的原始位置，一般是软件作者的主页
        MAINTAINER :  (optional) 软件包维护人员
        DEPENDS : (optional) 依赖项，运行本软件依赖的其他包
    endif
    ```

2. 软件包安装

    ```Makefile
    define Package/install
            软件包的安装方法，包括一系列拷贝编译好的文件到指定位置。調用時會帶一
            個參數，就是嵌入系統的鏡像文件系統目錄，因此$(1)表示嵌入系统的镜像目
            录。一般可以采用下面的方法：
            $(INSTALL_DIR)  $(1)/usr/sbin
            $(INSTALL_BIN)  $(PKG_BUILD_DIR)/$(PKG_NAME)  $(1)/usr/sbin/
    endif
    ```

### 其他BuildPackage相关的宏定义

列出的三个是在 Ser2TCP 这个模块下面用的宏定义，其他还有很多其他的定义，我们也可以了解下，再其他情况可能也会用到。

1. 编译准备
    ```Makefile
    define  Build/Prepare (可选)
        对于网上下载的软件包不需要再描述，对于非网上下载或自行开发的软件包
        必须说明编译准备方法，如下：  
        mkdir -p $(PKG_BUILD_DIR)
        创建编译目录，也就是$(TOPDIR)/build_dir/target-<ARCH>*/$(PKG_NAME)-%(PKG_VERSION)
        $(CP) ./src/* $(PKG_BUILD_DIR)/   
        将软件包的src的源码文件拷贝到编译目录去
    endif
    ```

2. 编译源代码,在该工程中，采用src问价夹中的 Makefile，所以注释了编译源代码的设置
    ```Makefile
    define  Build/Compile (可选)
        默认是编译源码里面的Makefile，如果你想传递一些参数比如环
        境变量什么的，那就可以定义，编译方法，没有特别说明的可以不予以定义。如果不
        定义将使用默认的编译方法Build/Compile/Default
        自行开发的软件包可以考虑使用下面的定义。
        $(MAKE) -C  $(PKG_BUILD_DIR)    \
                    $(TARGET_CONFIGURE_OPTS)  \
                    CFLAGS="$(TARGET_CFLAGS)  -I$(LINUX_DIR)/include"
    endif
    ```

## src 文件夹

该文件夹就采用普通的 Makefile 进行项目组织。非常简单易懂。
在这里我们用双线程将SOCKET通道和串口进行对接。

### socket 配置相关的操作

源码如下，配置参考注释：

```C
sockfd = socket(AF_INET, SOCK_STREAM, 0); /*建立socket*/
    if (sockfd == -1)
    {
        printf("socket failed:%d", errno);
    }

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(DEST_PORT); /*设置服务器端口，uint16_t形式的整数*/
    dest_addr.sin_addr.s_addr = inet_addr(DEST_IP); /*设置IP地址，IP地址只能是地址，不能是域名，字符串方式输入*/
    bzero(&(dest_addr.sin_zero), 8);           /* 其余的网络配置结构须置 0*/

    if (connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr)) == -1)
    {                                        //连接方法，传入句柄，目标地址和大小
        printf("connect failed:%d", errno); //失败时可以打印errno
    }
    else
    {
        //main loop
    }
```

### 串口配置相关的操作

源码如下，先定义了几个函数，配置参考注释：

```C
//打开串口
int open_port(void)
{
    int fd; //串口的标识符
    //O_NOCTTY用来告诉Linux这个程序不会成为“控制终端”
    //O_NDELAY用来告诉Linux这个程序不关心DCD信号
    fd = open(serial_device, O_RDWR | O_NOCTTY | O_NDELAY); //serial_device 为字符串，"/dev/ttyS1"
    if (fd == -1)
    {
        //不能打开串口
        perror("open_port: Unable to open /dev/ttyS0 -");
        return (fd);
    }
    else
    {
        fcntl(fd, F_SETFL, 0);
        printf("open ttys1 .....\n");
        return (fd);
    }
}

void set_speed_and_parity(int fd, int speed)
{
    struct termios options;  // 串口配置结构体
    tcgetattr(fd, &options); //获取当前设置
    bzero(&options, sizeof(options));
    options.c_cflag |= B115200 | CLOCAL | CREAD; // 设置波特率，本地连接，接收使能
    options.c_cflag &= ~CSIZE;                     //屏蔽数据位
    options.c_cflag |= CS8;                         // 数据位为 8 ，CS7 for 7
    options.c_cflag &= ~CSTOPB;                     // 一位停止位， 两位停止为 |= CSTOPB
    options.c_cflag &= ~PARENB;                     // 无校验
                                                 //options.c_cflag |= PARENB; //有校验
    //options.c_cflag &= ~PARODD // 偶校验
    //options.c_cflag |=  PARODD    // 奇校验
    options.c_cc[VTIME] = 10;          // 等待时间，单位百毫秒 （读）。后有详细说明
    options.c_cc[VMIN] = 255;          // 最小字节数 （读）。后有详细说明
    tcflush(fd, TCIOFLUSH);              // TCIFLUSH刷清输入队列。
    tcsetattr(fd, TCSANOW, &options); // TCSANOW立即生效；
}

int serialport()
{
    int fd;
    //打开串口
    if ((fd = open_port()) < 0)
    {
        perror("open_port error");
        return 0;
    }
    //设置波特率和校验位
    set_speed_and_parity(fd, 115200);
    return (fd);
}

```

之后再main函数中对这几个函数进行调用。

```C
int fd = serialport(); //初始化串口

```

再创建多个线程 并 定义线程传参的结构体，将串口和TCP连接怼起来。

```C

struct thread_info{
        int sockfd;
        int fd;
        char* buff;
};

void Ser2TCP(int sockfd, int fd, char *buff)
{
        int nread;
        while (1)
        {
                //读取串口
                nread = read(fd, buff, 128);
                if (nread > 0)
                {
                        // printf("nLen %dn\n", nread);
                        buff[nread + 1] = 0;
                        // printf("n%s\n", buff);
                        // TCP发送数据
                        send(sockfd, buff, nread, 0);
                }
                else
                {
                        sleep(1);
                }
        }
}

void Ser2TCP_thread(void *Tinfo){
        Ser2TCP(((struct thread_info *)Tinfo)->sockfd, ((struct thread_info *)Tinfo)->fd,((struct thread_info *)Tinfo)->buff);
}

void TCP2Ser(int sockfd, int fd, char *buff)
{
        int nread;
        while (1)
        {
                // TCP 接收数据
                sleep(1);
                nread = recv(sockfd, buff, 1024, 0); //将接收数据打入buf，参数分别是句柄，储存处，最大长度，其他信息（设为0即可）。
                // printf("TCP nLen %dn\n", nread);
                //写回串口
                write(fd, buff, nread);
        }
}
void TCP2Ser_thread(void *Tinfo){
        TCP2Ser(((struct thread_info *)Tinfo)->sockfd,((struct thread_info *)Tinfo)->fd,((struct thread_info *)Tinfo)->buff);
}
```

最后，利用多线程完成程序调度。

```C
        //main loop
        while (1)
        {
                // Ser2TCP(sockfd,fd,buff);
                Tinfo1.buff = buff;
                Tinfo1.fd = fd;
                Tinfo1.sockfd = sockfd;
                Tinfo2 = Tinfo1;
                Tinfo2.buff = buff_out;
                printf("connect success\n");
                if (pthread_create(&t0, NULL, (void *)& Ser2TCP_thread, (void* ) &Tinfo1) == -1) //创建 Ser2TCP_thread
                {
                        printf("fail to create pthread t0\n");
                        exit(1);
                }
                if (pthread_create(&t1, NULL, (void *)& TCP2Ser_thread, (void* ) &Tinfo2) == -1) //创建 TCP2Ser_thread
                {
                        printf("fail to create pthread t0\n");
                        exit(1);
                }

                pthread_join(t0, NULL);
                pthread_join(t1, NULL);
        }

```

## file 文件夹

明白了顶层的Makefile之后就可以了解（line40 - 55），在软件安装阶段，会将该文件夹的文件拷贝到目标目录中。关于LUCI的配置和启动相关的文件都可以在这里修改。

# 参考资料

1. [Openwrt package Makefile](https://www.jianshu.com/p/3320deb24335)
2. [Linux termios 串口编程之 VTIME与VMIN](https://blog.csdn.net/jazzsoldier/article/details/55508227)
3. [c 语言tcp socket 示例从简单到复杂](https://blog.csdn.net/tzshlyt/article/details/53229878)

# 交叉编译

## TK_SER2TCP_串口转TCP模块

该项目完成于浙江爱迪曼（天控科技）暑期社会实践期间，主要目的是完成OpenWRT操作系统中从串口到TCP端口的透传。运行的目标芯片为 MT7628，是一款优秀的路由器芯片。

天控科技的流量计通过串口将要发送的命令给路由器芯片，路由器芯片将串口收到的数据发送给远端TCP server。

路由器芯片只起到一个流量转发的作用。

### 文件组织

在该项目src中有三个文件

1. tcpclient.c

2. tcpclient.py

3. tcpserver.py

其中 tcpclient.c 为该项目主要程序。tcpserver.py 是服务器端执行的python3脚本，搭建一个简单的tcp环境用于测试。tcpclient.py 测试代码，验证TCP server是否工作。

### 使用方法

该文件在 Openwrt系统上运行，需要交叉编译,因為採用了多线程。交叉编译指令如下：

```shell
mipsel-openwrt-linux-gcc tcpclient.c -o testtcp.out -lpthread -ldl
```

生成的 testtcp.out 可执行文件可以直接放在 Openwrt 系统中执行。
