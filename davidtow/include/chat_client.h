#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include "chat_machine.h"

class chat_client: public chat_machine {
    
    int LOGGED_IN;
	
	int server_socket;
	
	struct sockaddr_in local_addr;
	struct sockaddr_in server_addr;
	
	int IP_SOCKET;
	struct sockaddr_in IP_ADDR;
	char remoteIP[INET6_ADDRSTRLEN];
	
	int FD_MAX;
	fd_set living_fds;
	fd_set readfds;
	
	char input[BUFFERSIZE];
	int nbytes;
	
	char LIST_PRINTABLE[600];
	
	void create_server_socket();
	void bind_socket_port();
	void set_server_addr(char* server_ip, char* server_port);
	void connect_to_server();
	
	void stdin_handler();
	void server_handler();
	
    public:
    chat_client(char* port);
    ~chat_client();
    
    void main();
	
    void login(char* server_ip, char* server_port);
    void send(char* client_ip, char* message);
    void broadcast(char* message);
    void block(char* client_ip);
    void unblock(char* client_ip);
    
    void refresh();
    void logout();
	void print_list();
	void exit_program();
    
};

#endif