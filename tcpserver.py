#!/usr/bin/python
# -*- coding: utf-8 -*-
import socket
HOST = ''                              # 为空代表所有可用的网卡
PORT = 50007                           # 任意非特权端口
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST, PORT))
s.listen(10)   # 最大连接数
conn, addr = s.accept()   # 返回客户端地址
print ('Connected by', addr)
while 1:
    data = conn.recv(1)             # 每次最大接收客户端发来数据1024字节
    if not data: break                 # 当没有数据就退出死循环 
    print (data)           # 打印接收的数据
    # conn.sendall(data.encode())                 # 把接收的数据再发给客户端
conn.close()