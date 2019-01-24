.PHONY:all
all:TcpClient TcpServer

TcpClient:TcpClient.cc
	g++ -o $@ $^ -static
TcpServer:TcpServer.cc
	g++ -o $@ $^ -std=c++11 -lpthread

.PHONY:clean
clean:
	rm -f TcpClient TcpServer
