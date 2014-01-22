/*
 * MessagingClient.cpp
 *
 *  Created on: Jan 20, 2014
 *      Author: chris
 */

#include "MessagingClient.h"

namespace std {

/**
 * Only constructor provided
 * @param string host
 * @param int port
 */
MessagingClient::MessagingClient(string host, int port, bool passedDebug):InetClient(host, port) {
	debug = passedDebug;
}

/**
 * default destructor
 */
MessagingClient::~MessagingClient() {}

/**
 * Starts messaging client by creating a socket connecction and passing control to the MessagingUI
 */
void MessagingClient::run(){
	create();
	messagingStart();
}


/**
 * Implements loop for request/response handling
 * Uses MessagingUI to collect input from the user and to display responses
 */
void MessagingClient::messagingStart(){
    string line;

    // loop to handle user interface
    while (1) {
    	line = messagingUI.getInput();
    	if(line == "")
    		break;

        // append a newline
        line += "\n";

        if(debug)
        	cout << "Request Sending:\n" << line << "NEWLINE NOW\n";  //  TODO for debugging

        // send request
        bool success = send_request(line);
        // break if an error occurred
        if (not success)
            break;
        // get a response
        success = getResponse();  //TODO there's an error here somewhere!!!
        // break if an error occurred
        if (not success)
            break;
    }
    close_socket();
}


/**
 * Overrides server.h get_request to provide for waiting for a file of a certain
 * length
 * @param int client socket identifier
 * @return string recieved from client
 */
bool MessagingClient::getResponse(){
	 string request = "";
	 bool isPutRequest = false;
	// read until we get a newline
	while (request.find("\n") == string::npos) {
		int nread = recv(server_,buf_,1024,0);
		if (nread < 0) {
			if (errno == EINTR)
				// the socket call was interrupted -- try again
				continue;
			else
				// an error occurred, so break out
				return false;
		} else if (nread == 0) {
			// the socket is closed
			return false;
		}
		// be sure to use append in case we have binary data
		request.append(buf_,nread);

		isPutRequest = finishReadingIfPutRequest(request);
		if(isPutRequest){
			printServerResponse(request);
			return true;
		}
	}

	printServerResponse(request);  //  TODO I should probably do something more intelligent here... later!
	return true;
}

/**
 * Checks to see if the input stream is a put request,
 * if so it reads in the length of the message to be send and continues to recieve
 * until the entire message has been recieved
 * @param int client socket indentifier
 * @param string the request string recieved so far -this string is appended if it's a put request
 * @return bool if the request is a put request
 */
bool MessagingClient::finishReadingIfPutRequest(string &request){
	stringstream requestStream(request);
	string tempCommand = "", tempSubject = "", tempLength = "", tempMessage = "", tempStr = "";

	if(!(requestStream >> tempCommand))  //  leave error checking to later functions
		return false;
	if(tempCommand != "message")  //  verification is only needed for put requests TODO
		return false;
	if(!(requestStream >> tempSubject))
		return false;
	if(!(getline(requestStream, tempLength)))
		return false;

	while(getline(requestStream, tempStr)){  //  get entire message up to this point
		tempMessage += tempStr;
		tempMessage += "\n";
	}

	int recievedMessageLength = atoi(tempLength.c_str());
	if(recievedMessageLength >= 0 && tempMessage.length() < recievedMessageLength){
		string newMessagePart = "";
		while ((tempMessage.length() + newMessagePart.length()) < recievedMessageLength) {
			        int nread = recv(server_,buf_,1024,0);
			        if (nread < 0) {
			            if (errno == EINTR)
			                // the socket call was interrupted -- try again
			                continue;
			            else
			                // an error occurred, so break out
			                return false;
			        } else if (nread == 0) {
			            // the socket is closed
			            return false;
			        }
			        // be sure to use append in case we have binary data
			        newMessagePart.append(buf_,nread);
		}

		request += newMessagePart;
		return true;
	}else{
		return false;  //  leave error handling for other functions TODO
	}
}

/**
 *  Overrides the client printServerReponse section and passes control
 *  to the messagingUI for correct formatting
 *  @param string server response
 */
void MessagingClient::printServerResponse(string response){
	if(debug)
		cout << "Response Recieved:\n" << response << "NEWLINE NOW\n";  //  TODO for debugging

	messagingUI.printResponse(response);
}

} /* namespace std */
