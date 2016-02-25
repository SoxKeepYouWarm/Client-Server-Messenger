
#include "chat_client.h"

chat_client::chat_client(char* port) {
    PORT = port;
	printf("chat client was just initialized\n");
}

chat_client::~chat_client() {
    
}


void chat_client::create_server_socket() {
	
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
      printf("Cannot create socket\n");
      exit(1);
	}
	
}


void chat_client::set_server_addr(char* server_ip, char* server_port) {
	
	// clear server_addr
   memset(&server_addr, 0, sizeof(server_addr));
   
   server_addr.sin_family = AF_INET;
   server_addr.sin_addr.s_addr = inet_addr(server_ip);
   server_addr.sin_port = htons(atoi(server_port));
   
   printf("server_addr was just set\n");
}


void chat_client::connect_to_server() {
	
   if (connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
      printf("Cannot connect to server\n");
      //exit(1);
   } else {
	   printf("client connected OK\n");
   }
   
   FD_SET(server_socket, &living_fds);
   FD_MAX = server_socket;
   
}


void chat_client::stdin_handler() {
	
	printf("input from stdin detected\n");
	int read_in = read(STDIN, input, BUFFERSIZE);
    if (read_in <= 0) {
		printf("error reading from stdin\n");	
	} else {
		handle_input(input);
		memset(&input, 0, sizeof input);
	}
					
}


void chat_client::server_handler() {
	
    if ((nbytes = recv(server_socket, input, sizeof input, 0)) <= 0) {
		// got error or connection closed by client
		if (nbytes == 0) {
			// connection closed
			printf("selectserver: socket %d hung up\n", server_socket);
		} else {
			perror("recv");
		}
		close(server_socket); // bye!
		FD_CLR(server_socket, &living_fds); // remove from master set
	} else {
		// we got some data from the server
		printf("message received was: %s\n", input);
        
	}				
}


void chat_client::main() {
	printf("running main in chat client\n");
	
    create_server_socket();
	
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
	
}


void chat_client::login(char* server_ip, char* server_port) {
    printf("login called in chat_client.cpp\n");
	this->set_server_addr(server_ip, server_port);
	this->connect_to_server();
}

void chat_client::send(char* client_ip, char* message) {
    
	if (FD_ISSET(server_socket, &living_fds)) {
		if (::send(server_socket, message, strlen(message), 0) == -1) {
			printf("error sending message\n");
		} else {
			printf("message sent successfully\n");
		}
	}
}

void chat_client::broadcast(char* message) {
    
}

void chat_client::block(char* client_ip) {
    
}

void chat_client::unblock(char* client_ip) {
    
}

void chat_client::refresh() {
    
}

void chat_client::logout() {
    
}
