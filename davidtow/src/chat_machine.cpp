#include "chat_machine.h"

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
	cse4589_print_and_log("I, davidtow, have read and understood the course academic integrity policy.\n");
	cse4589_print_and_log("[%s:END]\n", "AUTHOR");
}

void chat_machine::print_list() {
    printf("print list needs to be implemented.cpp\n");
}

void chat_machine::exit_program() { printf(MESSAGE); }


void chat_machine::tokenize_command(char* command, char* COMMAND, char* ARG_ONE, char* ARG_TWO) {
	
	std::string command_str(command);
	if (!command_str.empty() && command_str[command_str.length()-1] == '\n') {
    	command_str.erase(command_str.length()-1);
	}
		
	std::cout << "command_str is: " << command_str << std::endl;
	
	int start_index = 0;
	for (int i = 0; i < command_str.length(); i++) {
		
		if (command_str[i] == ' ' || command_str[i + 1] == '\0') {
			
			int offset_null = (command_str[i + 1] == '\0');
			std::string word_str = command_str.substr(start_index, (i - start_index) + offset_null);
			
			printf("word_str length is: %d\n", word_str.size());
			
			if (COMMAND[0] == '\0') {
				
				printf("word_str is %s\n", word_str.c_str());
				if (word_str.size() <= 32){
					strcpy(COMMAND, word_str.c_str());
				} else {
					COMMAND = "ERROR";
				}
				
			} else if (ARG_ONE[0] == '\0') {
				
				printf("word_str is %s\n", word_str.c_str());
				std::string comm(COMMAND);
				int max_size = 32;
				if (comm == "BROADCAST") { max_size = 256; }
				
				if (word_str.size() <= max_size){
					strcpy(ARG_ONE, word_str.c_str());
				} else { 
					COMMAND = "ERROR"; 
				}
				
			} else if (ARG_TWO[0] == '\0') {
				
				printf("word_str is %s\n", word_str.c_str());
				std::string comm(COMMAND);
				printf("just initiated string\n");
				int max_size = 32;
				if (comm == "SEND") { max_size = 256; }
				
				std::string remaining_string = command_str.substr(start_index);
				
				if (remaining_string.size() <= max_size){
					printf("about to copy arg_two\n");
					strcpy(ARG_TWO, remaining_string.c_str());
					printf("just copied arg_two\n");
				} else { 
					COMMAND = "ERROR"; 
				}

			} else {
				break;
			}
			
			while(command_str[++i] == ' ' );
			start_index = i;
		}
	}
	
}


void chat_machine::tokenize_request(char* request, char* COMMAND, char* ARG_ONE, char* ARG_TWO) {
	
	std::string input(request);
	if (!input.empty() && input[input.length()-1] == '\n') {
    	input.erase(input.length()-1);
	}
		
	int start = 0;
	for (int i = 0; input[i] != 0; i++) {
		
		if (input[i] == -1) {
			
			std::string clause = input.substr(start, (i - start));
			
			if (COMMAND[0] == 0) {
				strcpy(COMMAND, clause.c_str());
				printf("COMMAND is: %s\n", COMMAND);
			} else if (ARG_ONE[0] == 0) {
				strcpy(ARG_ONE, clause.c_str());
				printf("ARG_ONE is: %s\n", ARG_ONE);
			} else if (ARG_TWO[0] == 0) {
				strcpy(ARG_TWO, clause.c_str());
				printf("ARG_TWO is: %s\n", ARG_TWO);
			}
			
			start = i + 1;
		}
		
	}
	
}


void chat_machine::stringify_command(char* string, int string_len, 
			char* COMMAND, char* ARG_ONE, char* ARG_TWO) {
	
	printf("in stringify_command\n");
	
	char break_msg[2];
	break_msg[0] = -1;
	break_msg[1] = '\0';
	
	//memset(&string, 0, string_len);
	
	strcpy(string, COMMAND);
	strcat(string, break_msg);
	if (ARG_ONE) {
		strcat(string, ARG_ONE);
		strcat(string, break_msg);
		if (ARG_TWO) {
			strcat(string, ARG_TWO);
			strcat(string, break_msg);
		}
	}
	
}


void chat_machine::find_my_ip(char* buffer, size_t buflen) {
	
	printf("find my ip was called\n");
	
	int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("8.8.8.8");
    serv.sin_port = htons((uint16_t) 53);

    int err = connect(sock, (const sockaddr*) &serv, sizeof(serv));
	if (err == -1) {
		printf("there was an error with connect\n");
	}

    sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (sockaddr*) &name, &namelen);
    if (err == -1) {
		printf("there was an error with getsockname\n");
	}

    const char* ip = inet_ntop(AF_INET, &name.sin_addr, buffer, buflen);
	
	printf("my ip is: \"%s\"\n", ip);

    close(sock);
	
	strcpy(buffer, ip);		
}


void chat_machine::handle_input(char* input, int logged_in) {
    
	printf("initial command was: %s\n", input);
	
	char* COMMAND = new char[32]();
	char* ARG_ONE = new char[256]();
	char* ARG_TWO = new char[256]();
	
	tokenize_command(input, COMMAND, ARG_ONE, ARG_TWO);
	
	printf("COMMAND is: \"%s\" size: %d\n", COMMAND, std::string(COMMAND).length());
	printf("ARG_ONE is: \"%s\" size: %d\n", ARG_ONE, std::string(ARG_ONE).length());
	printf("ARG_TWO is: \"%s\" size: %d\n", ARG_TWO, std::string(ARG_TWO).length());
	
	// only login exit and author allowed when logged out
	
	std::string comm_str(COMMAND);
	
    if (str_equals(COMMAND, "AUTHOR")) {
        printf("COMMAND was AUTHOR\n");
        this->print_author();
    } else if (str_equals(COMMAND, "LOGIN")) {
        printf("COMMAND was LOGIN\n");
		this->login(ARG_ONE, ARG_TWO);
    } else if (str_equals(COMMAND, "EXIT")) {
        printf("COMMAND was EXIT\n");
    } else if (logged_in) {
		
		if (str_equals(COMMAND, "IP")) {
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
		} else {
			cse4589_print_and_log("[%s:ERROR]\n", COMMAND);
        	printf("did not recognize COMMAND\n");
    	}
		
	} else {
		cse4589_print_and_log("[%s:ERROR]\n", COMMAND);
        printf("did not recognize COMMAND\n");
    }
	
	delete [] COMMAND;
	delete [] ARG_ONE;
	delete [] ARG_TWO;
	
}
// COMMON METHODS    
