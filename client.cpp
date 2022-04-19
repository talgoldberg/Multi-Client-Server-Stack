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
#define port 8989
using namespace std;

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

int main()
{
    char buffer[8192];
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(clientSocket == -1)
        return 1;

    string ipAddress = "127.0.0.1";

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET,ipAddress.c_str(), &addr.sin_addr);

    int connectRes = connect(clientSocket,  (sockaddr*)&addr, sizeof(addr));
    if(connectRes == -1){
        cout<<"here the prob"<<endl;
        return 1;
    }

    string userInput;
    buffer[8192] = {0};

    
    do{
        getline(cin, userInput);

        int sendRes = send(clientSocket,userInput.c_str(),userInput.size() + 1, 0);
        if(sendRes == -1)
        {
            cout<< "Could not send to server"<<endl;
            continue;
        }
        if(userInput.compare("EXIT") == 0)
        {
            cout<< "Client Exit.."<<endl;
            break;
        }

        int valread = recv(clientSocket,buffer,8192,0);
        if(valread > 0)
        {
            std::vector<string> arr;
            string msg = string(buffer,0,valread);
            arr = split(msg," ");
            if(msg.compare("This is not a command") == 0 || msg.compare("You need text value to push to the stack") == 0
            || msg.compare("Cannot top from empty stack")==0 || msg.compare("Cannot pop empty stack")==0 || arr[0] == "OUTPUT:")
            {
                if(arr[0] == "OUTPUT:")
                {
                    cout<<msg<<endl;
                }
                else{
                    cout<<"Server Response: "<<msg<<endl;
                }

            }
            buffer[8192] = {0};
        }

    }while (true);

}