#include "client.h"

Client::Client() {
    // setup variables
	server_ = 0;
    buflen_ = 1024;
    buf_ = new char[buflen_+1];
}

Client::~Client() {
	delete []buf_;
}

void Client::run() {
    // connect to the server and run echo program
    create();
    echo();
}

void
Client::create() {
}

void
Client::close_socket() {
}

void
Client::echo() {
    string line;
    
    // loop to handle user interface
    while (getline(cin,line)) {
    	// ** let the use exit on EXIT
    	if(line == "EXIT")
    		break;
        // append a newline
        line += "\n";

        // send request
        bool success = send_request(line);
        // break if an error occurred
        if (not success)
            break;
        // get a response
        success = get_response();
        // break if an error occurred
        if (not success)
            break;
    }
    close_socket();
}

bool
Client::send_request(string request) {
    // prepare to send request
    const char* ptr = request.c_str();  // **send until sentinal
    int nleft = request.length();
    int nwritten;
    // loop to be sure it is all sent
    while (nleft) {
        if ((nwritten = send(server_, ptr, nleft, 0)) < 0) {
            if (errno == EINTR) {
                // the socket call was interrupted -- try again
                continue;
            } else {
                // an error occurred, so break out
                perror("write");
                return false;
            }
        } else if (nwritten == 0) {
            // the socket is closed
            return false;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return true;
}

bool
Client::get_response() {
    string response = "";
    // read until we get a newline
    while (response.find("\n") == string::npos) {  // **Receive until sentinel
        int nread = recv(server_,buf_,1024,0);  //  TODO fix spacing on send
        if (nread < 0) {
            if (errno == EINTR)  // **this must use global variables
                // the socket call was interrupted -- try again
                continue;
            else
                // an error occurred, so break out
                return "";
        } else if (nread == 0) {
            // the socket is closed
            return "";
        }
        // be sure to use append in case we have binary data
        response.append(buf_,nread);


    }

    printServerResponse(response);  //  TODO I should probably do something more intelligent here... later!
    return true;
}

/**
 * Print out the string returned by the server
 * @param string response from server
 */
void Client::printServerResponse(string response){
	cout << response;
}
