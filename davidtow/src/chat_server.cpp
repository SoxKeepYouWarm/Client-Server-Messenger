#include "chat_server.h"
#include <algorithm>

chat_server::chat_server(char* port) {
	memset(&buf, 0, sizeof buf);
    PORT = port;
	IP = new char[32];
	find_my_ip(IP, 32);
	memset(&input, 0, sizeof input);
}

chat_server::~chat_server() {
    
}


void chat_server::set_addr_info() {
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	int rv;
	if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}
	
}


void chat_server::create_listening_socket() {
	
	for(p = ai; p != NULL; p = p->ai_next) {
    	listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) { 
			continue;
		}
		
		int yes=1;        
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}

		break;
	}
	
	// if we got here, it means we didn't get bound
	if (p == NULL) {
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}

	freeaddrinfo(ai); // all done with this
	
}


void chat_server::listen_on_listening_socket() {
	
    if (listen(listener, MAXIMUM_CONNECTIONS) == -1) {
        perror("listen");
        exit(3);
    }

}


void chat_server::stdin_handler() {
	
	printf("input from stdin detected\n");
	int read_in = read(STDIN, input, BUFFERSIZE);
    if (read_in <= 0) {
		printf("error reading from stdin\n");	
	} else {
		handle_input(input);
		memset(&input, 0, sizeof input);
	}
					
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//in_port_t
unsigned short get_in_port(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return (((struct sockaddr_in*)sa)->sin_port);
    }

    return (((struct sockaddr_in6*)sa)->sin6_port);
}


void chat_server::listener_handler() {
	
    addrlen = sizeof remoteaddr;
	newfd = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen);
	if (newfd == -1) {
    	perror("accept");
    } else {
		
    	FD_SET(newfd, &master); // add to master set
        if (newfd > fdmax) {    // keep track of the max
			fdmax = newfd;
		}
		
		generate_list();
		
		if (FD_ISSET(newfd, &master)) {
		
			if (int out_bytes = ::send(newfd, LIST_PRINTABLE, 300, 0) == -1) {
				perror("send");
			} else {
				printf("new list sent %d bytes successfully\n", out_bytes);
			}
			
		}	
		
		
		int associated_socket = newfd;
		char client_ip[32] = "";
		//char client_port[32] = ""; 
		char hostname[32] = "";
		char client_port[32] = "";
		
		strcpy(client_ip, inet_ntop(remoteaddr.ss_family,
			::get_in_addr((struct sockaddr*)&remoteaddr),
			remoteIP, INET6_ADDRSTRLEN));
			
		//int temp_int = ntohs(get_in_port((struct sockaddr*)&remoteaddr));
		//std::string temp = toString(temp_int);
		//strcpy(client_port, temp.c_str());
			
		getnameinfo((struct sockaddr*)&remoteaddr, 
			sizeof remoteaddr, 
			hostname, sizeof hostname, client_port, sizeof client_port, 0);
			
		printf("service is %s\n", client_port);
			
		handle_login(associated_socket, client_ip, client_port, hostname);
			
			
	}
					
}


void chat_server::client_handler(int i) {
	
    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
		// got error or connection closed by client
		if (nbytes == 0) {
			// connection closed
			printf("selectserver: socket %d hung up\n", i);
			handle_logout(i);
		} else {
			perror("recv");
		}
		close(i);
		FD_CLR(i, &master); 
	} else {
		printf("Received %d bytes\n", nbytes);
		printf("Message received was: %s\n", buf);
		
		char* request = (char*) buf;
		proccess_request(i, request);
		
		memset(&buf, 0, sizeof buf);
	}				
}


void chat_server::proccess_request(int sender_socket, char* request) {
	
	char* sender_ip; 
	for (int i = 0; i < user_list.size(); i++) {
		if (user_list.at(i).associated_socket == sender_socket) {
			sender_ip = user_list.at(i).ip;
		}
	}
	
	char* COMMAND = new char[32]();
	char* ARG_ONE = new char[32]();
	char* ARG_TWO = new char[256]();
	
	tokenize_request(request, COMMAND, ARG_ONE, ARG_TWO);
	
	printf("command is: %s\n", COMMAND);
	printf("arg_one is: %s\n", ARG_ONE);
	printf("arg_two is: %s\n", ARG_TWO);
	
	if (str_equals(COMMAND, "SEND")) {
        printf("COMMAND was SEND\n");
        this->handle_send(sender_ip, ARG_ONE, ARG_TWO);
    } else if (str_equals(COMMAND, "BROADCAST")) {
		printf("COMMAND was BROADCAST\n");
		this->handle_broadcast(sender_socket, sender_ip, ARG_ONE);
	} else if (str_equals(COMMAND, "BLOCK")) {
		printf("COMMAND was BLOCK\n");
		this->handle_block(sender_socket, sender_ip, ARG_ONE);
	} else if (str_equals(COMMAND, "UNBLOCK")) {
		printf("COMMAND was UNBLOCK\n");
		this->handle_unblock(sender_socket, sender_ip, ARG_ONE);
	} else if (str_equals(COMMAND, "REFRESH")) {
		printf("COMMAND was REFRESH\n");
		this->handle_refresh(sender_socket);
	}
	
}


void chat_server::handle_login(int socket, char* ip, char* port, char* host) {
	
	user* this_user = NULL;
	for (int i = 0; i < user_list.size(); i++) {
		if (str_equals(user_list.at(i).ip, ip)) {
			this_user = &user_list.at(i);
		}
	}
	
	printf("user list size is %d\n", user_list.size());
	
	if (this_user == NULL) {
		// this is a new user
		
		struct user new_user;
		strcpy(new_user.ip, ip);
		strcpy(new_user.hostname, host);
		strcpy(new_user.remote_port, port);
		new_user.associated_socket = socket;
		
		user_list.push_back(new_user);
		
		printf("This is a new user\n");
		printf("new user ip is %s\n", ip);
		printf("new user port is %s\n", port);
		printf("new user host is \"%s\"\n", host);
		printf("new user list size is %d\n", user_list.size());
		
	} else {
		//this is a returning user
		printf("This is a returning user\n");
		printf("returning user ip is %s\n", this_user->ip);
		printf("returning user port is %s\n", this_user->remote_port);
		printf("returning user host is \"%s\"\n", this_user->hostname);
		
		// check if user has any pending messages
		if ( ! this_user->saved_messages.empty()) {
			// user has unreceived messages
			while ( ! this_user->saved_messages.empty()) {
				
				// get the most recent message
				message next_message = this_user->saved_messages.front();
				char* msg_sender = next_message.sender;
				char* msg_content = next_message.msg;
				
				char buffer[300] = "";
				sprintf(buffer, "msg from:%s\n[msg]:%s\n", msg_sender, msg_content);
				
				
				
				if (FD_ISSET(socket, &master)) {
					
					if (int out_bytes = ::send(socket, buffer, sizeof buffer, 0) == -1) {
						perror("send");
					} else {
						printf("message sent %d bytes successfully\n", out_bytes);
						
						// pop that message off the stack
						this_user->saved_messages.pop();
					}
				
				} else {
					printf("socket is not ready to receive stored messages\n");
				}
			}
		} 
		
	}
	
}


void chat_server::handle_logout(int socket) {
	
	for (int i = 0; i < user_list.size(); i++) {
		if (user_list.at(i).associated_socket == socket) {
			user_list.at(i).associated_socket = -1;
		}
	}
	
}


void chat_server::handle_send(char* sender_ip, char* target_ip, char* message) {
	printf("in handle_send target: %s and message: %s\n", target_ip, message);
	
	
	user* target_user;
	if (target_user = get_user_from_ip(target_ip)) {
		// user exists
		if ( std::find(target_user->black_list.begin(), 
			target_user->black_list.end(), sender_ip) != target_user->black_list.end() ) {
			// sender is blacklisted
			// HANDLE ERROR
		} else {
			// sender is not blacklisted			
			
			//increment their message received counter
			target_user->messages_received ++;
			
			int target_socket = target_user->associated_socket;
			
			if (target_socket == -1) {
				// user is offline, save message
				struct message new_message;
				strcpy(new_message.sender, sender_ip);
				strcpy(new_message.msg, message);
				
				target_user->saved_messages.push(new_message);
				
			} else {
				// user is online, send message
				
				char buffer[300] = "";
				sprintf(buffer, "msg from:%s\n[msg]:%s\n", sender_ip, message);
				
				if (FD_ISSET(target_socket, &master)) {
					
					if (int out_bytes = ::send(target_socket, buffer, 300, 0) == -1) {
						perror("send");
					} else {
						printf("message sent %d bytes successfully\n", out_bytes);
					}
				
				}
			
			}
			
		}
	} else {
		// user doesn't exist
		// HANDLE ERROR
	}
	
}


void chat_server::handle_broadcast(int sender_socket, char* sender_ip, char* message) {
	printf("in handle_broadcast sender: %s and message: %s", sender_ip, message);
	
	for (int i = 0; i < user_list.size(); i++) {
		if (user_list.at(i).associated_socket != sender_socket) {
			// every user except the sender
			user_list.at(i).messages_sent ++;
			handle_send(sender_ip, user_list.at(i).ip, message);
		}
	}
}


void chat_server::handle_block(int sender_socket, char* sender_ip, char* block_ip) {
	
	user* target_user;
	if (target_user = get_user_from_ip(block_ip)) {
		// target user exists
		user* sender_user = get_user_from_ip(sender_ip);
		if ( std::find(sender_user->black_list.begin(), 
			sender_user->black_list.end(), block_ip) != sender_user->black_list.end() ) {
			// user is already blocked
			// DO NOTHING
		} else {
			// user is not currently blocked
			sender_user->black_list.push_back(block_ip);
		}
		
	} else {
		// target user doesn't exist
		// HANDLE ERROR
	}
	
}


void chat_server::handle_unblock(int sender_socket, char* sender_ip, char* unblock_ip) {
	
	user* target_user;
	if (target_user = get_user_from_ip(unblock_ip)) {
		// target user exists
		user* sender_user = get_user_from_ip(sender_ip);
		
		// checking if user currently in blacklist
		int pos = std::find(sender_user->black_list.begin(), 
			sender_user->black_list.end(), unblock_ip) - sender_user->black_list.begin();
		if (pos < sender_user->black_list.size()) {
			// user is currently blocked
			sender_user->black_list.erase(sender_user->black_list.begin() + pos);
		} else {
			// user is not currently blocked
			// DO NOTHING
		}
		
	} else {
		// target user doesn't exist
		// HANDLE ERROR
	}
	
}


void chat_server::handle_refresh(int sender_socket) {
	
	generate_list();
	
	if (FD_ISSET(sender_socket, &master)) {
					
		if (int out_bytes = ::send(sender_socket, LIST_PRINTABLE, 300, 0) == -1) {
			perror("send");
		} else {
			printf("new list sent %d bytes successfully\n", out_bytes);
		}
				
	}
	
}


void chat_server::main() {
	printf("running main in chat server\n");
	
    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

	set_addr_info();
	create_listening_socket();
	listen_on_listening_socket();

	// register initial listeners
	FD_SET(STDIN, &master);
	fdmax = STDIN;
    FD_SET(listener, &master);
    fdmax = listener; 

    while (1) {
		
        read_fds = master; 
        
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        for(int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { 
                if (i == STDIN) {
					printf("hit stdin handler\n");
					stdin_handler();
			
				} else if (i == listener) {
                    printf("hit listener handler\n");
					listener_handler();
                
				} else {
                    printf("hit client handler\n");
					client_handler(i);
					
                }
            } 
        }
    }
	
	printf("outside while loop, this must be an error\n");
	
}


void chat_server::generate_list() {
	
	printf("currently inside generate_list\n");
	
	//std::vector<user> user_copy = user_list;
	std::sort(user_list.begin(), user_list.end());
	
	memset(&LIST_PRINTABLE, 0, sizeof LIST_PRINTABLE);
	
	for (int i = 0; i < user_list.size(); i++) {
		user* usr = &user_list.at(i);
		
		char line_buffer[100] = "";
		sprintf(line_buffer, 
			"%-5d%-35s%-20s%-8d\n", i, usr->hostname, usr->ip, atoi(usr->remote_port));
			
		strcat(LIST_PRINTABLE, line_buffer);
	}
	
}


void chat_server::print_list() {
	generate_list();
	cse4589_print_and_log("[%s:SUCCESS]\n", "LIST");
	cse4589_print_and_log(LIST_PRINTABLE);
	cse4589_print_and_log("[%s:END]\n", "LIST");
}


void chat_server::print_statistics() {
    printf("running statistics in chat server\n");
}


void chat_server::print_blocked(char* client_ip) {
    
}


void chat_server::exit_program() {
	printf("exit program was called\n");
	exit(0);
}


chat_server::user* chat_server::get_user_from_ip(char* ip) {
	
	for (int i = 0; i < user_list.size(); i++) {
		
		if (str_equals(user_list.at(i).ip, ip)) {
			return &user_list.at(i);
		}
		
	}
	
	return NULL;
	
}