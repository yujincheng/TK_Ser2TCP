#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdio.h>	 //标准输入输出定义
#include <stdlib.h>	//标准库函数定义
#include <unistd.h>	//UNIX标准函数定义
#include <sys/types.h> //基本系统数据类型
#include <sys/stat.h>  //获取一些文件相关的信息
#include <fcntl.h>	 //文件控制定义
#include <termio.h>
#include <errno.h>  //错误号定义
#include <stdio.h>  //标准输入输出定义
#include <stdlib.h> //标准函数库定义
#include <unistd.h> //Unix标准函数定义
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>   //文件控制定义
#include <termios.h> //POSIX中断控制定义
#include <errno.h>   //错误号定义
#include <string.h>

#include <pthread.h>

// #define DEST_PORT 9999 //目标地址端口号
// #define DEST_IP "61.153.226.170" /*目标地址IP，这里设为本机*/
// #define DEST_IP "101.6.65.194" /*目标地址IP，这里设为本机*/

// #define MAX_DATA 256 //接收到的数据最大程度
#define serial_device "/dev/ttyS1"

struct thread_info{
	int sockfd;
	int fd;
	char* buff;
};

//打开串口
int open_port(void)
{
	int fd; //串口的标识符
	//O_NOCTTY用来告诉Linux这个程序不会成为“控制终端”
	//O_NDELAY用来告诉Linux这个程序不关心DCD信号
	fd = open(serial_device, O_RDWR | O_NOCTTY | O_NDELAY);
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
	options.c_cflag &= ~CSIZE;					 //屏蔽数据位
	options.c_cflag |= CS8;						 // 数据位为 8 ，CS7 for 7
	options.c_cflag &= ~CSTOPB;					 // 一位停止位， 两位停止为 |= CSTOPB
	options.c_cflag &= ~PARENB;					 // 无校验
												 //options.c_cflag |= PARENB; //有校验
	//options.c_cflag &= ~PARODD // 偶校验
	//options.c_cflag |=  PARODD    // 奇校验
	options.c_cc[VTIME] = 6;		  // 等待时间，单位百毫秒 （读）。后有详细说明
	options.c_cc[VMIN] = 0;			  // 最小字节数 （读）。后有详细说明
	tcflush(fd, TCIOFLUSH);			  // TCIFLUSH刷清输入队列。
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

int main(int argc, char** argv)
{
	int sockfd, new_fd;			  /*cocket句柄和接受到连接后的句柄 */
	struct sockaddr_in dest_addr; /*目标地址信息*/
	struct thread_info Tinfo1, Tinfo2;
	int fd;
	int nread;
	char buff[512];
	char buff_out[512];
	pthread_t t0;
    pthread_t t1;

	uint16_t tcp_port = 9999;
	char* ip_addr = "101.6.65.194";

	if (1 == argc){
	}else if(2 == argc){
		printf("PLZ define ipaddr and port");
	}else if(3 <= argc){
		tcp_port = atoi(argv[2]);
		ip_addr = argv[1];
	}

	fd = serialport();

	sockfd = socket(AF_INET, SOCK_STREAM, 0); /*建立socket*/
	if (sockfd == -1)
	{
		printf("socket failed:%d\n", errno);
	}

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(tcp_port);
	dest_addr.sin_addr.s_addr = inet_addr(ip_addr);
	bzero(&(dest_addr.sin_zero), 8);

	if (connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr)) == -1)
	{										//连接方法，传入句柄，目标地址和大小
		printf("connect failed:%d\n", errno); //失败时可以打印errno
	}
	else
	{
		// Ser2TCP(sockfd,fd,buff);
		Tinfo1.buff = buff;
		Tinfo1.fd = fd;
		Tinfo1.sockfd = sockfd;
		Tinfo2 = Tinfo1;
		Tinfo2.buff = buff_out;
		printf("connect success\n");
		if (pthread_create(&t0, NULL, (void *)& Ser2TCP_thread, (void* ) &Tinfo1) == -1)
		{
			printf("fail to create pthread t0\n");
			exit(1);
		}
		if (pthread_create(&t1, NULL, (void *)& TCP2Ser_thread, (void* ) &Tinfo2) == -1)
		{
			printf("fail to create pthread t0\n");
			exit(1);
		}

		pthread_join(t0, NULL);
		pthread_join(t1, NULL);
	}
	close(sockfd); //关闭socket
	return 0;
}
