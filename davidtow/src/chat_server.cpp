#include "chat_server.h"

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


std::string toString(int val)
{
    std::stringstream stream;
    stream << val;
    return stream.str();
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
		
		int associated_socket = newfd;
		char client_ip[32] = "";
		char client_port[32]; 
		char hostname[32];
		char service[32];
		
		strcpy(client_ip, inet_ntop(remoteaddr.ss_family,
			::get_in_addr((struct sockaddr*)&remoteaddr),
			remoteIP, INET6_ADDRSTRLEN));
			
		int temp_int = ntohs(get_in_port((struct sockaddr*)&remoteaddr));
		std::string temp = toString(temp_int);
		strcpy(client_port, temp.c_str());
			
		getnameinfo((struct sockaddr*)&remoteaddr, 
			sizeof remoteaddr, 
			hostname, sizeof hostname, service, sizeof service, 0);
			
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
		
		
		
        for(int j = 0; j <= fdmax; j++) {
			// send to everyone!
			if (FD_ISSET(j, &master)) {
				// except the listener and ourselves
				if (j != listener) {
					if (int out_bytes = ::send(j, buf, nbytes, 0) == -1) {
						perror("send");
					} else {
						printf("message sent %d bytes successfully\n", nbytes);
					}
				}
			}
		}
		memset(&buf, 0, sizeof buf);
	}				
}


void chat_server::proccess_request(int sender_socket, char* request) {
	
	char* COMMAND = new char[32]();
	char* ARG_ONE = new char[32]();
	char* ARG_TWO = new char[256]();
	
	tokenize_request(request, COMMAND, ARG_ONE, ARG_TWO);
	
	if (str_equals(COMMAND, "SEND")) {
        printf("COMMAND was AUTHOR\n");
        this->handle_send(ARG_ONE, ARG_TWO);
    } else if (str_equals(COMMAND, "BROADCAST")) {
		this->handle_broadcast(sender_socket, ARG_TWO);
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
		printf("This is a new user\n");
		
		struct user new_user;
		strcpy(new_user.ip, ip);
		strcpy(new_user.hostname, host);
		strcpy(new_user.remote_port, port);
		new_user.associated_socket = socket;
		
		user_list.push_back(new_user);
		
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
			
		} 
		
	}
	
	printf("selectserver: new connection from %s on "
			"socket %d\n", ip, socket);
	printf("port is %s\n", port);
	printf("host is \"%s\"\n", host);
	
}


void chat_server::handle_send(char* target, char* message) {
	printf("in handle_send target: %s and message: %s", target, message);
}


void chat_server::handle_broadcast(int sender_socket, char* message) {
	printf("in handle_broadcast sender: %d and message: %s", sender_socket, message);
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


void chat_server::print_statistics() {
    printf("running statistics in chat server\n");
}

void chat_server::print_blocked(char* client_ip) {
    
}

void chat_server::exit_program() {
	printf("exit program was called\n");
	exit(0);
}