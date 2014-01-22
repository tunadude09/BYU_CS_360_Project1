/*
 * MessagingServer.h
 *
 *  Created on: Jan 20, 2014
 *      Author: chris
 */

#ifndef MESSAGINGSERVER_H_
#define MESSAGINGSERVER_H_

#include <vector>
#include <map>
#include "inet-server.h"

namespace std {

/**
 * A Message struct for clarity and convenience
 * Contains the subject, message, and message length
 */
struct Message{
	string subject;
	int length;
	string message;
};

/**
 * This class extends the basic internet server to add messaging capabilities
 * It stores a vector of users information in memory and allows sockets to update
 * modify, and retrieve that data
 * This class overrides server.h's handle function and then implements it's own functions
 * @author Chris Fortuna
 * @date 1/20/2014
 */
class MessagingServer: public InetServer {
public:
	/**
	 * Simple constructor which initializes the server port and sets all
	 * socketInputstrings to ""
	 * @param int port number
	 */
	MessagingServer(int port, bool passedDebug):InetServer(port){
		resetInput();
		debug = passedDebug;
	}

	/**
	 * Default do nothing constructor
	 */
	virtual ~MessagingServer(){}


private:
	virtual void handle(int client);
	virtual string getRequest(int);
	bool finishReadingIfPutRequest(int client, string &request);
	string processRequest(string request);
	bool addNewMessage(stringstream &request);
	string getMessageListForUser(stringstream &request);
	string getMessage(stringstream &request);
	bool eraseAllMessages(stringstream &request);
	void resetInput();

	map <string, map <int, Message> > users;
	string enteredCommand;
	string enteredUser;
	string enteredIndex;
	string enteredSubject;
	string enteredLength;
	string enteredMessage;
	bool debug;
};

} /* namespace std */

#endif /* MESSAGINGSERVER_H_ */
