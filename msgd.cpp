#include <stdlib.h>
#include <unistd.h>

#include <iostream>

#include "MessagingServer.h"

using namespace std;

int
main(int argc, char **argv)
{
    int option, port;

    // setup default arguments
    port = 3002;
    bool debug = false;

    // process command line options using getopt()
    // see "man 3 getopt"
    while ((option = getopt(argc,argv,"p:d::")) != -1) {
        switch (option) {
            case 'p':
                port = atoi(optarg);
                break;
            case 'd':
            	debug = true;
            	break;
            default:
                cout << "server [-p port]" << endl;
                exit(EXIT_FAILURE);
        }
    }

    MessagingServer server(port, debug);
    server.run();
}
