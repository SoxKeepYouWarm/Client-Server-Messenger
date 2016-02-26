#ifndef CHAT_MACHINE_H
#define CHAT_MACHINE_H

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define BUFFERSIZE 256
#define MAXIMUM_CONNECTIONS 10

#include "../include/global.h"
#include "../include/logger.h"

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

class chat_machine {
    
    public:
    //virtual chat_machine();
    virtual ~chat_machine();
    
    // COMMON METHODS
    virtual char* get_ip();
    virtual char* get_port();
    
    virtual void print_ip();
    virtual void print_port();
    virtual void print_author();
    virtual void print_list();
	virtual void exit_program();
	
	virtual void tokenize_command(char* input, 
		char* COMMAND, 
		char* ARG_ONE, 
		char* ARG_TWO);
	
	virtual void find_my_ip(char* buffer, size_t buflen);
	
	void handle_input(char* command);
	
	virtual void main() = 0;
    // COMMON METHODS
    
    // CLIENT METHODS
    virtual void login(char* server_ip, char* server_port);
    virtual void send(char* client_ip, char* message);
    virtual void broadcast(char* message);
    virtual void block(char* client_ip);
    virtual void unblock(char* client_ip);
    
    virtual void refresh();
    virtual void logout();
    // CLIENT METHODS
    
    // SERVER METHODS    
    virtual void print_statistics();
    virtual void print_blocked(char* client_ip);
    // SERVER METHODS
    
    protected:
    
    char* IP;
    char* PORT;
    
};

#endif