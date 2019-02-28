#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <sstream> 
#include <errno.h> 
#include <unistd.h> 

#include "httpRequest.h"
#include "httpRequest.cpp"

using namespace std; 


#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

void parseURL(char url[]); 

int main(int argc, char *argv[])
{    
    char *URL = "/index.html"; //default 
    if (argc > 1){
        URL = argv[1]; 
    }
    cout << "Search URL: " << URL << endl; 
    parseURL(URL);
    int sock02 = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servAddr; 
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(4000);     // short, network byte order
    servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(servAddr.sin_zero, '\0', sizeof(servAddr.sin_zero));

    //Connect to server 
    if (connect(sock02, (struct sockaddr *)&servAddr, sizeof(servAddr))) {
        perror("connect");
        return 2;
    }
    cout << ">> Successfully connected to server\n";

    //Now we need to create the HTTP Request and send it to the server
    //set host, etc.  
    //call buildRequest to form the message into a string as show: 
    //"GET /beej/inet_ntoaman.html http/1.1\nHOST: retran.com\n\n"
    cout << "Creating HTTP Request...\n";
    httpRequest req; 
    req.setMethod("GET");
    req.setHost("127.0.0.1");
    req.setUri(URL);
    req.setVer("HTTP/1.0");
    req.createRequest();
    cout << "==================\n";
    //send that to the server 
    int reqLen =0, res=0, sent=0;
    reqLen = req.getLength();
    char buffer[10054]; 
    do{
        res = send(sock02, req.convert().c_str(),(reqLen - sent),0);
        sent = sent + res; 
    } while (sent < reqLen);
    res = recv(sock02, buffer, 10054,0);
    if(res > 0){
        cout << buffer; 
    } else if(res == 0){
        cout << "Connection Closed\n";
    } else {
        cout << "recv failed\n";
    }
    //server returns code 
    //client processes code 
    //server sends file 
    


}

char host[20];
char port[10];
char parsed_url[20];  
void parseURL(char url[]){
    char sample; 
    
    int i=0,j=0,k=0,l=0; //counter

    while(sample != ' '){
        sample = url[i]; //take in char 
        cout << sample; 
        if(sample == ':' && url[i+1] == '/'){
            //we're two away from host name
            i=i+3; 
            while(sample != ':'){
                host[j] = url[i];  
                i++; j++; 
            } //now we have the host name 
            i++; 
            sample = url[i]; 
            while(sample != '/'){
                port[k] = url[i]; 
                k++; i++; 
            }//now we have port num 
            i++; 
            sample = url[i]; 
            while(sample != ' '){
                parsed_url[l] = url[i]; 
                l++; i++; 
            }
        }
        
        i++; 
    }
    cout << "Parsed data: " << host << port << parsed_url; 
}