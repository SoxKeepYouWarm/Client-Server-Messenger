
#include "chat_client.h"

chat_client::chat_client(char* port) {
    PORT = port;
	IP = new char[32];
	find_my_ip(IP, 32);
	
	memset(&input, 0, sizeof input);
	printf("chat client was just initialized\n");
}

chat_client::~chat_client() {

}


void chat_client::create_server_socket() {
	
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
      printf("Cannot create socket\n");
      exit(1);
	}
	
	int yes=1;        
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
	
	printf("just created server socket\n");
	
}


void chat_client::bind_socket_port() {
	
	memset(&local_addr, 0, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(atoi(PORT));

	if (bind(server_socket, 
			(struct sockaddr *) &local_addr, 
			sizeof(local_addr)) < 0) {
    	
		perror("bind");
    	exit(1);
	}
	
}


void chat_client::set_server_addr(char* server_ip, char* server_port) {
	
	// clear server_addr
   memset(&server_addr, 0, sizeof(server_addr));
   
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = inet_addr(server_ip);
   server_addr.sin_port = htons(atoi(server_port));
   
   printf("server_addr was just set to ip: \"%s\" and port: \"%s\"\n", server_ip, server_port);
   
}


void chat_client::connect_to_server() {
	
   	if (connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
		printf("Cannot connect to server\n");
   		perror("connect");
		cse4589_print_and_log("[%s:ERROR]\n", "LOGIN");
   	} else {
		printf("client connected OK\n");
		FD_SET(server_socket, &living_fds);
   		FD_MAX = server_socket;
		LOGGED_IN = 1;
	}
   
}


void chat_client::stdin_handler() {
	
	printf("input from stdin detected\n");
	int read_in = read(STDIN, input, BUFFERSIZE);
    if (read_in <= 0) {
		printf("error reading from stdin\n");	
	} else {
		handle_input(input, LOGGED_IN);
		memset(&input, 0, sizeof input);
	}
					
}


void chat_client::server_handler() {
	
    if ((nbytes = recv(server_socket, input, BUFFERSIZE, 0)) <= 0) {
		// got error or connection closed by client
		if (nbytes == 0) {
			// connection closed
			printf("selectserver: socket %d hung up\n", server_socket);
			LOGGED_IN = 0;
		} else {
			perror("recv");
		}
		close(server_socket); 
		FD_CLR(server_socket, &living_fds); 
	} else {
		// we got some data from the server
		
		if (input[0] == -1) {
			// this is a new list
			printf("new list\n");
			memset(&LIST_PRINTABLE, 0, 600);
			memcpy(LIST_PRINTABLE, &input[1], 600);
			print_list();
			
		} else {
			//regular message delivery
			cse4589_print_and_log("[%s:SUCCESS]\n", "RECEIVED");
			cse4589_print_and_log(input);
			cse4589_print_and_log("[%s:END]\n", "RECEIVED");
			
		}
		
		memset(&input, 0, sizeof input);
        
	}				
}


void chat_client::main() {
	printf("running main in chat client\n");
	
    create_server_socket();
	bind_socket_port();
	
	// clear read file descriptors
	FD_ZERO(&living_fds);
	FD_ZERO(&readfds);
	
	// set stdin for reading
	FD_SET(STDIN, &living_fds);
	FD_MAX = STDIN;
	
	while (1) {
		
		readfds = living_fds;
		select(FD_MAX+1, &readfds, NULL, NULL, NULL);
		
		for (int i = 0; i <= FD_MAX; i++) {
			
			if (FD_ISSET(i, &readfds)) {
				
				if (i == STDIN) {
					
					stdin_handler();
			
				} else if (i == server_socket) {
					
					server_handler();
					
				}
			}
		}
		
	}
	
	printf("outside while loop, this must be an error\n");
	
}


void chat_client::login(char* server_ip, char* server_port) {
    printf("login called in chat_client.cpp\n");
	this->set_server_addr(server_ip, server_port);
	this->connect_to_server();
}


void chat_client::send(char* client_ip, char* msg) {
	
	char* message = new char[320]();
	
	char* COMMAND = "SEND";
	char* ARG_ONE = client_ip;
	char* ARG_TWO = msg;
	
	stringify_command(message, 320, COMMAND, ARG_ONE, ARG_TWO);
	
	printf("sending message: %s\n", message);
	
	if (FD_ISSET(server_socket, &living_fds)) {
		if (int nbytes = ::send(server_socket, message, strlen(message), 0) == -1) {
			printf("error sending message\n");
		} else {
			printf("message sent %d bytes successfully\n", nbytes);
		}
	}
}

void chat_client::broadcast(char* msg) {
    
	char* message = new char[300]();
	
	char* COMMAND = "BROADCAST";
	char* ARG_ONE = msg;
	
	stringify_command(message, 300, COMMAND, ARG_ONE, NULL);
	
	printf("sending broadcast: %s\n", message);
	
	if (FD_ISSET(server_socket, &living_fds)) {
		if (int nbytes = ::send(server_socket, message, strlen(message), 0) == -1) {
			printf("error sending broadcast\n");
		} else {
			printf("broadcast sent %d bytes successfully\n", nbytes);
		}
	}
}

void chat_client::block(char* client_ip) {
    
	char* message = new char[300]();
	
	char* COMMAND = "BLOCK";
	char* ARG_ONE = client_ip;
	
	stringify_command(message, 300, COMMAND, ARG_ONE, NULL);
	
	printf("sending block: %s\n", message);
	
	if (FD_ISSET(server_socket, &living_fds)) {
		if (int nbytes = ::send(server_socket, message, strlen(message), 0) == -1) {
			printf("error sending block\n");
		} else {
			printf("block sent %d bytes successfully\n", nbytes);
		}
	}
}

void chat_client::unblock(char* client_ip) {
    
	char* message = new char[300]();
	
	char* COMMAND = "UNBLOCK";
	char* ARG_ONE = client_ip;
	
	stringify_command(message, 300, COMMAND, ARG_ONE, NULL);
	
	printf("sending unblock: %s\n", message);
	
	if (FD_ISSET(server_socket, &living_fds)) {
		if (int nbytes = ::send(server_socket, message, strlen(message), 0) == -1) {
			printf("error sending unblock\n");
		} else {
			printf("block sent %d bytes successfully\n", nbytes);
		}
	}
	
}

void chat_client::refresh() {
    
	char* message = new char[300]();
	
	char* COMMAND = "REFRESH";
	
	stringify_command(message, 300, COMMAND, NULL, NULL);
	
	printf("sending refresh: %s\n", message);
	
	if (FD_ISSET(server_socket, &living_fds)) {
		if (int nbytes = ::send(server_socket, message, strlen(message), 0) == -1) {
			printf("error sending refresh\n");
		} else {
			printf("refresh sent %d bytes successfully\n", nbytes);
			cse4589_print_and_log("[%s:SUCCESS]\n", "REFRESH");
			cse4589_print_and_log("[%s:END]\n", "REFRESH");
		}
	}
}

void chat_client::logout() {
    if (LOGGED_IN) {
		close(server_socket); 
		FD_CLR(server_socket, &living_fds); 
		printf("user logged out\n");
		LOGGED_IN = 0;
	} else {
		printf("user is not currently logged in\n");
	}
	
	create_server_socket();
	bind_socket_port();
	
}


void chat_client::print_list() {
	cse4589_print_and_log("[%s:SUCCESS]\n", "LIST");
	cse4589_print_and_log(LIST_PRINTABLE);
	cse4589_print_and_log("[%s:END]\n", "LIST");
}


void chat_client::exit_program() {
	printf("exit program was called\n");
	this->logout();
	exit(0);
}
