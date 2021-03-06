#ifndef CHAT_MACHINE_H
#define CHAT_MACHINE_H

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#define BUFFERSIZE 601
#define MAXIMUM_CONNECTIONS 10

#define SEND_RESP -2
#define BLOCK_RESP -3
#define UNBLOCK_RESP -4
#define NO_QUEUED_MESSAGES -5 

#define OK 0
#define ERROR -1

#include "../include/global.h"
#include "../include/logger.h"
#include "utility.h"

#include <iostream>

#include <sstream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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
	
	virtual void tokenize_command(char* command, 
		char* COMMAND, 
		char* ARG_ONE, 
		char* ARG_TWO);
		
	virtual void tokenize_request(char* request, 
		char* COMMAND, 
		char* ARG_ONE, 
		char* ARG_TWO);
		
	virtual void stringify_command(char* string, int string_len, 
			char* COMMAND, char* ARG_ONE, char* ARG_TWO);
	
	virtual void find_my_ip(char* buffer, size_t buflen);
	
	void handle_input(char* command, int logged_in);
	
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