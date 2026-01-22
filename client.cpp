#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <sys/socket.h>
#include<string>
#include<sys/select.h>
using namespace std;
int main()
{
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
    {
        perror("socket");
        return 1;
    }

    struct sockaddr_in servrAddr;
    memset(&servrAddr, 0, sizeof(servrAddr));
    servrAddr.sin_family = AF_INET;
    servrAddr.sin_port = htons(8080);
    servrAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // server's ip address

    socklen_t len = sizeof(servrAddr);
    char buffer[1024];
    if (connect(socketfd, (struct sockaddr *)&servrAddr, sizeof(servrAddr)) < 0)
    {
        perror("connect");
        close(socketfd);
        return 1;
    }
    string inbuf = "";
    while (true)
    {
        
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);
        FD_SET(socketfd, &readfds);

        int maxfd = max(0, socketfd) + 1;

        int ready = select(maxfd, &readfds, NULL, NULL, NULL);

        if(ready<0)
        {
            perror("Select");
            break;
        }
        
        if(FD_ISSET(0, &readfds))
        {
            string msg;
            if(!getline(cin, msg))
                break;
            
            //getline(cin, msg);
            msg.push_back('\n');

            if(send(socketfd, msg.c_str(), msg.size(), 0) <= 0)
            {
                break;
            }

        }

        if(FD_ISSET(socketfd, &readfds))
        {
            int n = recv(socketfd, buffer, sizeof(buffer), 0);

            if(n>0)
            {
                inbuf.append(buffer, n);

                int pos;
                while((pos = inbuf.find('\n')) != string::npos)
                {
                    string line = inbuf.substr(0, pos);
                    inbuf.erase(0, pos+1);

                    if(line == "Exit")
                    {
                        write(1, "Server Disconnected!", 20);
                        close(socketfd);
                        return 0;
                    }
                    
                    cout<<line<<endl;
                }
            }
            else
            {
                cout<<"Server closed connection\n"<<endl;
                break;
            }
        }

    }
        
}
