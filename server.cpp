#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <string>

using namespace std;

int main()
{
    int socketfd;

    socketfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servrAddr;
    memset(&servrAddr, 0, sizeof(servrAddr));

    servrAddr.sin_family = AF_INET;
    servrAddr.sin_port = htons(8080);
    servrAddr.sin_addr.s_addr = INADDR_ANY;

    struct sockaddr_in clientAddr;

    bind(socketfd, (struct sockaddr *)&servrAddr, sizeof(servrAddr));
    listen(socketfd, 7);
    
    
    int clientfd = -1;
    string inbuf;
    while (true)
    {

        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);
        FD_SET(socketfd, &readfds);

        int maxfd = socketfd;

        if(clientfd!=-1)
        {
            FD_SET(clientfd, &readfds);
            maxfd = max(maxfd, clientfd);
        }

        int ready = select(maxfd + 1, &readfds, NULL, NULL, NULL);

        if(ready<0)
        {
            perror("select");
            break;
        }

        if(FD_ISSET(socketfd, &readfds) && clientfd == -1)
        {
            socklen_t len = sizeof(clientAddr);
            clientfd = accept(socketfd, (struct sockaddr *)&clientAddr, &len);
            if(clientfd>=0){
                cout<<"New client connected"<<endl;
                inbuf.clear();
                
            }
        }

        if(FD_ISSET(0, &readfds) && clientfd!=-1)
        {
            string msg;
            if(!getline(cin, msg))
                break;
            
            msg.push_back('\n');
            send(clientfd, msg.c_str(), msg.size(), 0);
        }

        if(FD_ISSET(clientfd, &readfds) && clientfd!=-1)
        {
            char recv_buffer[1024];
            int n = recv(clientfd, recv_buffer, sizeof(recv_buffer), 0);
            if(n<=0)
            {
                cout<<"Client got disconnected"<<endl;
                close(clientfd);
                clientfd = -1;
            }
            else{
                
                inbuf.append(recv_buffer, n);
                int pos;
                while((pos = inbuf.find('\n')) != string::npos)
                {
                    string line = inbuf.substr(0, pos);
                    inbuf.erase(0, pos+1);

                    if(line == "Exit")
                    {
                        cout<<"Client Disconnected!"<<endl;
                        clientfd = -1;
                        close(clientfd);
                        break;
                    }

                    cout<<"Client: "<<line<<endl;
                }
            }
        }
        
        //socklen_t len = sizeof(clientAddr);
        //int clientfd = accept(socketfd, (struct sockaddr *)&clientAddr, &len);
        

        // write(1, "Client Disconnected!", 20);
        // write(1, "\n", 1);

        // close(clientfd);
    }
}