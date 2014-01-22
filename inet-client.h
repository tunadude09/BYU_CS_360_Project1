#pragma once

#include <netinet/in.h>

#include "client.h"

class InetClient : public Client {

public:
    InetClient(string, int);
    virtual ~InetClient();

protected:
    void create();
    void close_socket();

private:
    string host_;
    int port_;
};
