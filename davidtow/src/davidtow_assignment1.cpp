/**
 * @davidtow_assignment1
 * @author  David Towson <davidtow@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <iostream>
#include <stdio.h>

#include "../include/global.h"
#include "../include/logger.h"
#include "../include/utility.h"

#include "../include/chat_server.h"
#include "../include/chat_client.h"
#include "../include/chat_machine.h"

using namespace std;

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
 
 chat_machine* my_machine;
 
 char* MY_PORT;
 int IS_SERVER;
 
 
int identify_machine(int argc, char **argv) {
    
    if (argc != 3) {
        printf("incorrect input, please input the c or s for client or server, followed by the port number to operate on\n");
        return -1;
    } else {
        if (argv[1][0] == 's') {
            printf("program was started as server\n");
            return 1;
        } else if(argv[1][0] == 'c') {
            printf("program was started as client\n");
            return 0;
        } else {
            printf("neither c nor s was detected as first input\n");
        }
    }
}
 
 
int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/* Clear LOGFILE*/
    fclose(fopen(LOGFILE, "w"));
    
    printf("argc is %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("arg is %s\n", argv[i]);
    }
   
	MY_PORT = argv[2];
	
    IS_SERVER = identify_machine(argc, argv);
    
    if (IS_SERVER == 1) {
        my_machine = new chat_server(MY_PORT);
    } else {
        my_machine = new chat_client(MY_PORT);
    }
    
	my_machine->main();
	
	return 0;
}
