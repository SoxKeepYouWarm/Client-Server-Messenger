#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include "chat_machine.h"

class chat_client: public chat_machine {
    
    int LOGGED_IN;
	
	int server_socket;
	struct sockaddr_in server_addr;
	
	int FD_MAX;
	fd_set living_fds;
	fd_set readfds;
	
	char input[BUFFERSIZE];
	int nbytes;
    
	void create_server_socket();
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
    
};

#endif