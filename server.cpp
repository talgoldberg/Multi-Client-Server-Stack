#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <sys/time.h>
#include <vector>
#include "lockfreestack.h"
#include "SynchronisedStack.h"

#define TRUE 1

using namespace std;

#define PORT 8989
LockfreeStack<string> MyStack;
SynchronisedStack<string> SynStack;

void InsertToStack(string s)
{

    SynStack.Push(s);
}

void ConsumeFromStack(string& res)
{

    string s;
    if(SynStack.Size()==0)
    {
        s = "Cannot pop empty stack";
        res = s;
    }
    else
    {
        SynStack.TryPop();
    }

}

void PeekFromStack(string& res)
{
    string s;
    if(SynStack.Size()==0)
    {
        s = "Cannot top from empty stack";
        res = s;
    }
    else
    {
        SynStack.TryTop(s);
        res = s;

    }
}
std::vector<string> split(string str,string sep){
    char* cstr=const_cast<char*>(str.c_str());
    char* current;
    std::vector<string> arr;
    current=strtok(cstr,sep.c_str());
    while(current!=NULL){
        arr.push_back(current);
        current=strtok(NULL,sep.c_str());
    }
    return arr;
}

void Practic_6(int bytesrecv,int sd,int i,char buffer[8192],char buffer1[8192],char buffer2[8192],char buffer3[8192],char buffer4[8192],char buffer5[8192])
{


    std::vector<string> arr;
    string msg = string(buffer,0,bytesrecv);
    arr=split(msg," ");

    if(arr[0] != "PUSH" && arr[0] != "POP" && arr[0] != "TOP" && arr[0] != "EXIT")
    {
        cout<< "Client["<<i<< "]: " <<msg << endl;
        string errmsg  = "This is not a command";
        memcpy(buffer1,errmsg.c_str(),errmsg.size()+1);
        send(sd,buffer1,strlen(buffer1),0);

    }
    if(arr[0] == "PUSH" )
    {
        if(arr.size()==1)
        {
            cout<< "Client["<<i<< "]: " <<msg << endl;
            string errmsg  = "You need text value to push to the stack";
            memcpy(buffer2,errmsg.c_str(),errmsg.size()+1);
            send(sd,buffer2,strlen(buffer2),0);
        }
        else
        {
            cout<< "Client["<<i<< "]: " <<msg << endl;
            std::thread startInsertIntoQueue = std::thread(InsertToStack,arr[1].c_str());
            startInsertIntoQueue.join();
            send(sd,buffer,strlen(buffer),0);
        }

    }

    if(arr[0] == "POP")
    {
        cout<< "Client["<<i<< "]: " <<msg << endl;
        string res;
        std::thread consumeFromQueue = std::thread(ConsumeFromStack,std::ref(res));
        consumeFromQueue.join();
        if(res.compare("Cannot pop empty stack")==0)
        {
            memcpy(buffer4,res.c_str(),res.size()+1);
            send(sd,buffer4,strlen(buffer4),0);
        }
        else
        {
            send(sd,buffer,strlen(buffer),0);
        }

    }

    if(arr[0] == "TOP")
    {

        cout<< "Client["<<i<< "]: " <<msg << endl;
        string res;
        std::thread peekFromQueue = std::thread(PeekFromStack,std::ref(res));
        peekFromQueue.join();
        if(res.compare("Cannot top from empty stack")==0)
        {
            memcpy(buffer3,res.c_str(),res.size()+1);
            send(sd,buffer3,strlen(buffer3),0);
        }
        else
        {
            string output = "OUTPUT: ";
            output.append(res);
            memcpy(buffer5,output.c_str(),output.size()+1);
            send(sd,buffer5,strlen(buffer5),0);
        }
    }
}

void Practic_8(int bytesrecv,int sd,int i,char buffer[8192],char buffer1[8192],char buffer2[8192],char buffer3[8192],char buffer4[8192],char buffer5[8192])
{

    std::vector<string> arr;
    string msg = string(buffer,0,bytesrecv);
    arr=split(msg," ");

    if(arr[0] != "PUSH" && arr[0] != "POP" && arr[0] != "TOP" && arr[0] != "EXIT")
    {
        cout<< "Client["<<i<< "]: " <<msg << endl;
        string errmsg  = "This is not a command";
        memcpy(buffer1,errmsg.c_str(),errmsg.size()+1);
        send(sd,buffer1,strlen(buffer1),0);

    }

    if(arr[0] == "PUSH" )
    {
        if(arr.size()==1)
        {
            cout<< "Client["<<i<< "]: " <<msg << endl;
            string errmsg  = "You need text value to push to the stack";
            memcpy(buffer2,errmsg.c_str(),errmsg.size()+1);
            send(sd,buffer2,strlen(buffer2),0);
        }
        else
        {
            cout<< "Client["<<i<< "]: " <<msg << endl;
            MyStack.push(arr[1].c_str());
            send(sd,buffer,strlen(buffer),0);
        }

    }

    if(arr[0] == "POP")
    {
        cout<< "Client["<<i<< "]: " <<msg << endl;


        bool res = MyStack.pop();
        if(res)
        {
            send(sd,buffer,strlen(buffer),0);
        }
        else
        {
            string err = "Cannot pop empty stack";
            memcpy(buffer4,err.c_str(),err.size()+1);
            send(sd,buffer4,strlen(buffer4),0);
        }

    }

    if(arr[0] == "TOP")
    {

        cout<< "Client["<<i<< "]: " <<msg << endl;

        auto res = MyStack.top();
        if(res == nullptr)
        {
            string err = "Cannot top from empty stack";
            memcpy(buffer3,err.c_str(),err.size()+1);
            send(sd,buffer3,strlen(buffer3),0);
        }
        else
        {
            string output = "OUTPUT: ";
            output.append(*res);
            memcpy(buffer5,output.c_str(),output.size()+1);
            send(sd,buffer5,strlen(buffer5),0);
        }

    }
}




int main(int argc, char *argv[])
{

    int clientSocket[30];
    int max_sd;
    int max_clients = 30;
    int sd;
    int activity;
    int newSocket;
    int i;
    int addrlen;
    int opt = TRUE;
    fd_set  readfds;
    char buffer[8192];
    char buffer1[8192];
    char buffer2[8192];
    char buffer3[8192];
    char buffer4[8192];
    char buffer5[8192];

    for(i = 0; i< max_clients; i++)
    {
        clientSocket[i] = 0;
    }

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(serverSocket == -1){
        cerr << "Could not create a socket !"<< endl;
    }
    if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0){
        cerr << "setsockopt"<< endl;
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);

    if(bind(serverSocket, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        cerr << "bind failed"<< endl;
        exit(EXIT_FAILURE);
    }

    if(listen(serverSocket,3) == 0)
    {
        cout<< "[+]Server is ready for clients to connect on port number "<< PORT << endl;
    }

    addrlen = sizeof(addr);

    while(TRUE)
    {
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);
        max_sd = serverSocket;

        for(i = 0; i< max_clients; i++)
        {
            sd = clientSocket[i];

            if(sd > 0)
                FD_SET(sd, &readfds);
            if(sd > max_sd)
                max_sd = sd;
        }

        activity = select(max_sd + 1, &readfds, NULL,NULL,NULL);

        if(activity < 0 && (errno!=EINTR))
        {
            cout << "select error" << endl;
        }
        if(FD_ISSET(serverSocket, &readfds)){
            if((newSocket = accept(serverSocket,(struct sockaddr*)&addr, (socklen_t*)&addrlen))<0){
                cout<<"Accept"<<endl;
            }
            cout<<"New connection, socket fd: "<< newSocket<< " ip is: "<< inet_ntoa(addr.sin_addr) << " port: " << ntohs(addr.sin_port)<<endl;
            for(i = 0; i< max_clients; i++){
                if( clientSocket[i] == 0){
                    clientSocket[i] = newSocket;
                    break;
                }
            }
        }

        for(i = 0; i< max_clients; i++){
            sd =  clientSocket[i];
            if(FD_ISSET(sd,&readfds)){

                memset(buffer,0,8192);
                memset(buffer1,0,8192);
                memset(buffer2,0,8192);
                memset(buffer3,0,8192);
                memset(buffer4,0,8192);
                memset(buffer5,0,8192);
                int bytesrecv = recv(sd,buffer,8192,0);
                if(bytesrecv == -1)
                {
                    cerr << "Error in recv()"<< endl;
                    break;
                }
                if(bytesrecv == 0)
                {
                    close(sd);
                    clientSocket[i] = 0;
                    cout<<"Client["<<i<< "]: Disconnected"<<endl;
                    break;
                }
                else{

                    if(bytesrecv > 0){
                        //Practic_6(bytesrecv,sd,i,buffer,buffer1,buffer2,buffer3,buffer4,buffer5);
                        Practic_8(bytesrecv,sd,i,buffer,buffer1,buffer2,buffer3,buffer4,buffer5);
                    }
                }
            }
        }
    }


}