#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include "chat_machine.h"

#include <queue>
#include <vector>

class chat_server: public chat_machine {
    
	struct message {
		char sender[32];
		char msg[256];
	};
	
	struct user {
		char ip[32];
		char hostname[32];
		char remote_port[32];
		char black_list[32][32];
		int associated_socket;
		std::queue<message> saved_messages;
	};
	
	std::vector<user> user_list;
	
	char input[BUFFERSIZE];
	
	int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
    int nbytes;
	
	char remoteIP[INET6_ADDRSTRLEN];
	int remote_port;
	
	fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number
    
	struct addrinfo hints, *ai, *p;
	
	void set_addr_info();
	void create_listening_socket();
	void listen_on_listening_socket();
	
	void stdin_handler();
	void listener_handler();
	void client_handler(int i);
	
	void proccess_request(int sender_socket, char* request);
	void handle_login(int socket, char* ip, char* port, char* host);
	void handle_logout(int socket);
	void handle_send(char* target, char* message);
	void handle_broadcast(int sender_socket, char* message);
	
    public:
    chat_server(char* port);
    ~chat_server();
    
    void main();

    void print_statistics();
    void print_blocked(char* client_ip);
	
	void exit_program();
    
};

#endif