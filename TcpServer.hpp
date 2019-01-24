#ifndef _TCP_SERVER_HPP_
#define _TCP_SERVER_HPP_

#include <iostream>
#include <string>
#include <map>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "ThreadPool.hpp"

class TcpServer;


std::map<std::string, std::string> dict;

class Arg{
    public:
        std::string ip;
        int port;
        int sock;
        TcpServer *tsp;
};

class TcpServer{
    private:
        int listen_sock;
        int port;
        ThreadPool pool;
    public:
        TcpServer(int port_):port(port_),listen_sock(-1)
        {}
        void InitServer()
        {
            listen_sock = socket(AF_INET, SOCK_STREAM, 0);
            if(listen_sock < 0){
                std::cerr << "socket error" << std::endl;
                exit(2);
            }

            int opt = 1;
            setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

            struct sockaddr_in local;
            bzero(&local, sizeof(local));
            local.sin_family = AF_INET;
            local.sin_port = htons(port);
            local.sin_addr.s_addr = htonl(INADDR_ANY);
            //local.sin_addr.s_addr = inet_addr(ip.c_str());
            if(bind(listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0){
                std::cerr << "bind error" << std::endl;
                exit(3);
            }
            if(listen(listen_sock, 5) < 0){
                std::cout << "listen error" << std::endl;
                exit(4);
            }
            signal(SIGCHLD, SIG_IGN);
            pool.InitThreadPool();
        }
        static void Service(int sock)
        {
            while(1){
                char buf[1024];
                ssize_t s = read(sock, buf, sizeof(buf)-1);
                if(s > 0){
                    buf[s] = 0;
                    auto it = dict.find(buf);
                    if(it == dict.end()){
                        write(sock, "NULL", strlen("NULL"));
                        continue;
                    }
                    write(sock, (it->second).c_str(), (it->second).size());
                }
                else if(s == 0){
                    std::cout << "client quit!" << std::endl;
                    break;
                }
                else{
                    std::cerr << "read error" << std::endl;
                    break;
                }
            }
            close(sock);
        }
        void Start()
        {
            struct sockaddr_in peer;
            socklen_t len;
            for(;;){
                len = sizeof(peer);
                int sock = accept(listen_sock, (struct sockaddr*)&peer, &len);
                if(sock < 0){
                    std::cerr << "accept error" << std::endl;
                    continue;
                }
                std::cout << "Get a new client " << sock << std::endl;

                Task t;
                t.InitTask(sock, TcpServer::Service);
                pool.PushTask(t);
              //  int port_ = ntohs(peer.sin_port);
              //  std::string ip_ = inet_ntoa(peer.sin_addr);

              //  pthread_t tid;
              //  Arg *arg = new Arg;
              //  arg->sock = sock;
              //  arg->ip = ip_;
              //  arg->port = port_;
              //  arg->tsp = this;
              //  pthread_create(&tid, NULL, HandlerRequest, (void *)arg);


                //pid_t id = fork();
                //if(id < 0){
                //    std::cerr << "fork error" << std::endl;
                //    close(sock);
                //    //TODO
                //    continue;
                //}
                //else if( id == 0 ){
                //    close(listen_sock);
                //    Service(sock, ip_, port_);
                //    exit(0);
                //}
                //else{
                //    //child
                //    close(sock);
                //}
            }
        }
        ~TcpServer()
        {
            if(listen_sock > 0)
                close(listen_sock);
        }
};

#endif

