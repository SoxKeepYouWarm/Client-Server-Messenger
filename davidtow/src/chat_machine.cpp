#include <stdio.h>
#include <string.h>
#include "chat_machine.h"
#include "utility.h"

char* MESSAGE = "This function cannot be performed on this machine\n";

chat_machine::~chat_machine(){};

// CLIENT METHODS
void chat_machine::login(char* server_ip, char* server_port) { printf(MESSAGE); }
void chat_machine::send(char* client_ip, char* message) { printf(MESSAGE); }
void chat_machine::broadcast(char* message) { printf(MESSAGE); }
void chat_machine::block(char* client_ip) { printf(MESSAGE); }
void chat_machine::unblock(char* client_ip) { printf(MESSAGE); }
    
void chat_machine::refresh() { printf(MESSAGE); }
void chat_machine::logout() { printf(MESSAGE); }
// CLIENT METHODS
    
// SERVER METHODS    
void chat_machine::print_statistics() { printf(MESSAGE); }
void chat_machine::print_blocked(char* client_ip) { printf(MESSAGE); }
// SERVER METHODS
    
// COMMON METHODS
char* chat_machine::get_ip() {
    return IP;    
}

char* chat_machine::get_port() {
    return PORT;
}
    
void chat_machine::print_ip() {
    printf("IP:%s\n", IP);
}

void chat_machine::print_port() {
    printf("PORT:%s\n", PORT);
}

void chat_machine::print_author() {
    printf("I, David Towson, have read and understood the course academic integrity policy.\n");
}

void chat_machine::print_list() {
    printf("printing list in chat_machine.cpp\n");
}

void chat_machine::handle_input(char* command) {
    
	printf("initial command was: %s\n", command);
	
	char* tokens = strtok(command, " ");
	
	char* COMMAND = "";
	char* ARG_ONE = "";
	char* ARG_TWO = "";
	
	while (tokens != NULL)
  	{
		if (COMMAND[0] == '\0') {
			COMMAND = tokens;
		} else if (ARG_ONE[0] == '\0') {
			ARG_ONE = tokens;
		} else if (ARG_TWO[0] == '\0') {
			ARG_TWO = tokens;
		} else {
			printf("error parsing command\n");
		}
    	tokens = strtok (NULL, " ");
  	}
	
	printf("command is: %s\n", COMMAND);
	printf("arg_one is: %s\n", ARG_ONE);
	printf("arg_two is: %s\n", ARG_TWO);
	
    if (str_equals(COMMAND, "AUTHOR")) {
        printf("COMMAND was AUTHOR\n");
        this->print_author();
    } else if (str_equals(COMMAND, "IP")) {
        printf("COMMAND was IP\n");
        this->print_ip();
    } else if (str_equals(COMMAND, "PORT")) {
        printf("COMMAND was PORT\n");
		this->print_port();
    } else if (str_equals(COMMAND, "LIST")) {
        printf("COMMAND was LIST\n");
        this->print_list();
    } else if (str_equals(COMMAND, "STATISTICS")) {
        printf("COMMAND was STATISTICS\n");
		this->print_statistics();
    } else if (str_equals(COMMAND, "BLOCKED")) {
        printf("COMMAND was BLOCKED\n");
		this->print_blocked(ARG_ONE);
    } else if (str_equals(COMMAND, "LOGIN")) {
        printf("COMMAND was LOGIN\n");
		this->login(ARG_ONE, ARG_TWO);
    } else if (str_equals(COMMAND, "REFRESH")) {
        printf("COMMAND was REFRESH\n");
		this->refresh();
    } else if (str_equals(COMMAND, "SEND")) {
        printf("COMMAND was SEND\n");
		this->send(ARG_ONE, ARG_TWO);
    } else if (str_equals(COMMAND, "BROADCAST")) {
        printf("COMMAND was BROADCAST\n");
		this->broadcast(ARG_ONE);
    } else if (str_equals(COMMAND, "BLOCK")) {
        printf("COMMAND was BLOCK\n");
		this->block(ARG_ONE);
    } else if (str_equals(COMMAND, "UNBLOCK")) {
        printf("COMMAND was UNBLOCK\n");
		this->unblock(ARG_ONE);
    } else if (str_equals(COMMAND, "LOGOUT")) {
        printf("COMMAND was LOGOUT\n");
		this->logout();
    } else if (str_equals(COMMAND, "EXIT")) {
        printf("COMMAND was EXIT\n");
		//exit 0;
    } else {
        printf("did not recognize COMMAND\n");
    }
}
// COMMON METHODS    
