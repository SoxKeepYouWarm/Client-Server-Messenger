#include "chat_server.h"

chat_server::chat_server(char* port) {
    PORT = port;
	IP = new char[32];
	find_my_ip(IP, 32);
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


void chat_server::listener_handler() {
	
    addrlen = sizeof remoteaddr;
	newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
	if (newfd == -1) {
    	perror("accept");
    } else {
		
    	FD_SET(newfd, &master); // add to master set
        if (newfd > fdmax) {    // keep track of the max
			fdmax = newfd;
		}
		printf("selectserver: new connection from %s on "
			"socket %d\n",
			inet_ntop(remoteaddr.ss_family,
			::get_in_addr((struct sockaddr*)&remoteaddr),
			remoteIP, INET6_ADDRSTRLEN),
			newfd);
			
	}
					
}


void chat_server::client_handler(int i) {
	
    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
		// got error or connection closed by client
		if (nbytes == 0) {
			// connection closed
			printf("selectserver: socket %d hung up\n", i);
		} else {
			perror("recv");
		}
		close(i); // bye!
		FD_CLR(i, &master); // remove from master set
	} else {
		// we got some data from a client
		printf("message received was: %s\n", buf);
        for(int j = 0; j <= fdmax; j++) {
			// send to everyone!
			if (FD_ISSET(j, &master)) {
				// except the listener and ourselves
				if (j != listener) {
					if (::send(j, buf, nbytes, 0) == -1) {
						perror("send");
					}
				}
			}
		}
		memset(&buf, 0, sizeof buf);
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

    // main loop
    while (1) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == STDIN) {
					
					stdin_handler();
			
				} else if (i == listener) {
                    
					listener_handler();
                
				} else {
                    
					client_handler(i);
					
                }
            } 
        }
    }
}


void chat_server::print_statistics() {
    printf("running statistics in chat server\n");
}

void chat_server::print_blocked(char* client_ip) {
    
}
