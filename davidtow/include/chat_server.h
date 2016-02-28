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
		int messages_sent;
		int messages_received;
		std::vector<std::string> black_list;
		int associated_socket;
		std::queue<message> saved_messages;
		
		bool operator < (const user& another_user) const {
        	return (atoi(remote_port) < atoi(another_user.remote_port));
    	}
	};
	
	std::vector<user> user_list;
	char LIST_PRINTABLE[300];
	
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
	void handle_send(char* sender_ip, char* target, char* message);
	void handle_broadcast(int sender_socket, char* sender_ip, char* message);
	void handle_block(int sender_socket, char* sender_ip, char* block_ip);
	void handle_unblock(int sender_socket, char* sender_ip, char* block_ip);
	void handle_refresh(int sender_socket);
	
	user* get_user_from_ip(char* ip); 
	
    public:
    chat_server(char* port);
    ~chat_server();
    
    void main();

	void generate_list();
	void print_list();
    void print_statistics();
    void print_blocked(char* client_ip);
	
	void exit_program();
    
};

#endif