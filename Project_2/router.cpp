#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string> 
#include <iostream>
#include <sstream> 
#include <errno.h> 
#include <unistd.h> 
#include "router.h"
using namespace std; 

int nTable[size][size] = {0,1,1,0, 1,0,0,1, 1,0,0,1, 0,1,1,0}; 

router::router(){ //default constructor 
    this->name = ' '; 
    this->port = 10000; 
    for(int i =0; i< size; i++){
        this->neighbours[i] = 0; 
    } 
}

void router::initialise(char n, int p){ // initialises address settings 
    this->name = n;
    this->port = p;
    for(int i =0; i< size; i++){
        this->neighbours[i] = 0; 
    }

    memset(&servAddr, 0, sizeof(servAddr));
    memset(&cliAddr, 0, sizeof(cliAddr));

    this->servAddr.sin_family = AF_INET;
    this->servAddr.sin_port = htons(this->port);     
    this->servAddr.sin_addr.s_addr = INADDR_ANY;

    this->len = sizeof(this->cliAddr); 
    this->sockc = socket(AF_INET, SOCK_DGRAM, 0);
    this->socks = socket(AF_INET, SOCK_DGRAM, 0);

   // cout << "\n\nRouter Name: "<< this->name << "\nPort: " << this->port << endl; 
}

//SETTERS 
void router::setName(char n){
    this->name = n; 
}

void router::setPort(int p){
    this->port = p; 
}

void router::setNeighbours(int source){
    //char abc[4] = {'A','B','C','D'};
    for(int i = 0; i<size; i++){
        for(int j = 0; j< size ;j++){
            if(source == i && nTable[source][j] == 1){
                //found a neighbour 
                this->neighbours[j] = 1; 
                //cout << abc[j] << " is a neighbour.\n"; 
            }
        }
    }
}

//GETTERS 
char router::getN(){
    return this->name; 
}

int router::getP(){
    return this->port; 
}

int router::getANeighbour(int index){
    return this->neighbours[index]; 
}


void router::Rsend(char msg[100]){    
    //CLIENT SEND 
    cout << "Client thread running\n";
    char recvmsg[100]; //receive buffer 
    int s,r; 
    
    s = sendto( (this->sockc), msg, 100, MSG_CONFIRM, ( struct sockaddr *)(this->pSer), (this->len));
    if(s == -1){
        perror("send error");
    }
    cout << "Message sent -[ " << msg << " ]-" << endl; 

        //CLIENT RECEIVE 
    r = recvfrom((this->sockc), (char *)recvmsg, 100, MSG_WAITALL, ( struct sockaddr *)(this->pCli), this->plen);
    if(r == -1){
        perror("recv error");
    }
    cout << "Client receiving response: " << recvmsg << endl; 
}

void router::Rrecv(){
    char recvmsg[100]; 
    char sendmsg[100] = "SERV-RESP";
    int r,s; 
    cout << "Server thread running\n";
    if(bind(this->socks, (const struct sockaddr *)(this->pSer), sizeof(this->servAddr)) <0){
        perror("bind error"); 
    }
    
    while(1){
    //SERVER RECEIVE 
        r = recvfrom(this->socks, (char *)recvmsg, 100, MSG_WAITALL, 
                        (struct sockaddr *)(this->pCli), this->plen);
        if(r == -1){
            perror("recv error");
        }
        cout << "Server receives " << recvmsg << "...sending response "<<sendmsg<<  endl; 
            

        //SERVER SEND RESPONSE 
        s = sendto(this->socks, sendmsg, 100, 0, (const struct sockaddr *)(this->pCli), this->len);
        if(s == -1){
            perror("send error");
        }
    }
}