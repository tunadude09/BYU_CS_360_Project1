#pragma once

#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>

using namespace std;

class Server {
public:
    Server();
    virtual ~Server();

    void run();
    
protected:
    virtual void create();
    virtual void close_socket();
    void serve();
    virtual void handle(int);
    virtual string get_request(int);
    bool send_response(int, string);

    int server_;
    int buflen_;
    char* buf_;
};
