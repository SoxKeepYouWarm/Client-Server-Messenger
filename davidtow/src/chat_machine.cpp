#include <stdio.h>
#include <string.h>
#include "chat_machine.h"
#include "utility.h"

const char* MESSAGE = "This function cannot be performed on this machine\n";

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
    cse4589_print_and_log("[%s:SUCCESS]\n", "IP");
	cse4589_print_and_log("IP:%s\n", IP);
	cse4589_print_and_log("[%s:END]\n", "IP");
}

void chat_machine::print_port() {
	cse4589_print_and_log("[%s:SUCCESS]\n", "PORT");
	cse4589_print_and_log("PORT:%s\n", PORT);
	cse4589_print_and_log("[%s:END]\n", "PORT");
}

void chat_machine::print_author() {
	cse4589_print_and_log("[%s:SUCCESS]\n", "AUTHOR");
	cse4589_print_and_log("I, David Towson, have read and understood the course academic integrity policy.\n");
	cse4589_print_and_log("[%s:END]\n", "AUTHOR");
}

void chat_machine::print_list() {
    printf("print list needs to be implemented.cpp\n");
}


void chat_machine::tokenize_command(char* command, char* COMMAND, char* ARG_ONE, char* ARG_TWO) {
	
	std::string command_str(command);
	std::cout << "command_str is: " << command_str << std::endl;
	
	printf("BEFORE: command is: %s\n", COMMAND);
	printf("BEFORE: arg_one is: %s\n", ARG_ONE);
	printf("BEFORE: arg_two is: %s\n", ARG_TWO);
	
	int start_index = 0;
	for (int i = 0; i < command_str.length(); i++) {
		
		if (command_str[i] == ' ' || command_str[i + 1] == '\0') {
			
			int offset_null = (command_str[i + 1] == '\0');
			std::string word_str = command_str.substr(start_index, (i - start_index) + offset_null);
			
			std::cout << "word_str is: " << word_str << std::endl;
			
			if (COMMAND[0] == '\0') {
				strcpy(COMMAND, word_str.c_str());
			} else if (ARG_ONE[0] == '\0') {
				strcpy(ARG_ONE, word_str.c_str());
			} else if (ARG_TWO[0] == '\0') {
				
				std::string remaining_string = command_str.substr(start_index);
				std::cout << "remaining string is: " << remaining_string << std::endl;
				
				strcpy(ARG_TWO, remaining_string.c_str());

			} else {
				break;
			}
			
			while(command_str[++i] == ' ' );
			start_index = i;
		}
	}
	
}


void chat_machine::handle_input(char* input) {
    
	printf("initial command was: %s\n", input);
	
	char* COMMAND = new char[32];
	char* ARG_ONE = new char[32];
	char* ARG_TWO = new char[256];
	
	memset(COMMAND, 0, 32);
	memset(ARG_ONE, 0, 32);
	memset(ARG_TWO, 0, 256);
	
	printf("BEFORE CALL: command is: %s\n", COMMAND);
	printf("BEFORE CALL: arg_one is: %s\n", ARG_ONE);
	printf("BEFORE CALL: arg_two is: %s\n", ARG_TWO);
	
	tokenize_command(input, COMMAND, ARG_ONE, ARG_TWO);
	
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
    } else {
		cse4589_print_and_log("[%s:ERROR]\n", COMMAND);
        printf("did not recognize COMMAND\n");
    }
	
	delete [] COMMAND;
	delete [] ARG_ONE;
	delete [] ARG_TWO;
	
}
// COMMON METHODS    
