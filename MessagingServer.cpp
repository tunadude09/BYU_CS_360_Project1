/*
 * MessagingServer.cpp
 *
 *  Created on: Jan 20, 2014
 *      Author: chris
 */

#include <sstream>
#include <iostream>
#include "MessagingServer.h"

namespace std {

/**
 * Overrides server.h's handle in order to process and store messages
 * Outputs errors as well TODO this is gonna need a lot of work TODO
 * @param int client takes the int representation of a newly opened client socket
 */
void MessagingServer::handle(int client){
    // loop to handle all requests
    while (1) {
    	string request = "";

        if(debug)
        	cout << "Request Anticipated:\n" << request << "NEWLINE NOW\n";  //  TODO for debugging
        request = getRequest(client);  // TODO temporary
        if(debug)
        	cout << "Request Recieved:\n" << request << "NEWLINE NOW\n";  //  TODO for debugging
        if (request.empty())
            break;

    	resetInput();  //  TODO might want to do this somewhere else
        request = processRequest(request);
        if(request == "" || request == "\n"){
        	request = "error SOMETHING BAD HAPPENED (fix errors later)\n";
        }

        if(debug)
        	cout << "Response Sending:\n" << request << "NEWLINE NOW\n";  //  TODO for debugging

        // send response
        bool success = send_response(client,request);
        // break if an error occurred
        if (not success)
            break;
    }
    close(client);
}

/**
 * Overrides server.h get_request to provide for waitiing for a file of a certain
 * length
 * @param int client socket identifier
 * @return string recieved from client
 */
string MessagingServer::getRequest(int client){
	 string request = "";
	 bool isPutRequest = false;
	    // read until we get a newline
	    while (request.find("\n") == string::npos) {
	        int nread = recv(client,buf_,1024,0);
	        if (nread < 0) {
	            if (errno == EINTR)
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
	        request.append(buf_,nread);

	        isPutRequest = finishReadingIfPutRequest(client, request);
	        if(isPutRequest)
	        	return request;
	    }
	    // a better server would cut off anything after the newline and
	    // save it in a cache
	    return request;
}

/**
 * Checks to see if the input stream is a put request,
 * if so it reads in the length of the message to be send and continues to recieve
 * until the entire message has been recieved
 * @param int client socket indentifier
 * @param string the request string recieved so far -this string is appended if it's a put request
 * @return bool if the request is a put request
 */
bool MessagingServer::finishReadingIfPutRequest(int client, string &request){
	stringstream requestStream(request);
	string tempCommand = "", tempUser = "", tempSubject = "", tempLength = "", tempMessage = "", tempStr = "";

	if(!(requestStream >> tempCommand))  //  leave error checking to later functions
		return false;
	if(tempCommand != "put")  //  verification is only needed for put requests TODO
		return false;
	if(!(requestStream >> tempUser))
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
			        int nread = recv(client,buf_,1024,0);
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

/** FIXME
 * This will take the input and behave like the same from the client
 * This will call the appropriate action function for the server or return error etc
 * FIXME there's a lot to do here!!!!!
 */
string MessagingServer::processRequest(string request){
	stringstream requestStream(request);
	string messageString = "";

	requestStream >> enteredCommand;
	if(enteredCommand == ""){
		messageString = "";
		if(debug)  //  FIXME all these debugging errors might need to always display -ask
			cout << "error INCORRECT REQUEST COMMAND FORMATTING 1\n";

	}else if(enteredCommand == "put"){
		if(debug)
			cout << "put!\n";  //  TODO remove these later
		bool success = addNewMessage(requestStream);
		if(success)
			messageString = "OK";
		else
			if(debug)
				cout << "error INCORRECT REQUEST FORMATTING\n";

	}else if(enteredCommand == "list"){
		if(debug)
			cout << "list!\n";  //  TODO remove these later
		messageString = getMessageListForUser(requestStream);
		if(messageString == "")
			if(debug)
				cout << "error INCORRECT REQUEST FORMATTING\n";

	}else if(enteredCommand == "get"){
		if(debug)
			cout << "get!\n";  //  TODO remove these later
		messageString = getMessage(requestStream);
		if(messageString == "")
			if(debug)
				cout << "error INCORRECT REQUEST FORMATTING\n";

	}else if(enteredCommand == "reset"){  // if quit is followed by other characters TODO need to test
		if(debug)
			cout << "reset!\n";  //  TODO remove these later
		bool success = eraseAllMessages(requestStream);
		if(success)
			messageString = "OK";
		else
			if(debug)
				cout << "error INCORRECT REQUEST FORMATTING\n";

	}else{
		messageString = "";
		if(debug)
			cout << "error INCORRECT REQUEST COMMAND FORMATTING 2\n";
	}

	// TODO make a universal close of stringStream!!! If I need to?
	if(enteredCommand == "get")
		return messageString;
	else return messageString + "\n";
}

/**
 * Recieves a message to be stored
 * First makes sure that the recieved arguements are correct and then stores them into a new user
 * if none by that name exists yet or puts them into the message map for an existing user
 * This does no error checking for acceptable user/subject/length/or message
 * @param stringstream & request this is the stream used the process request function
 * @return bool to indicate message storage success or failure
 */
bool MessagingServer::addNewMessage(stringstream &request){
	if(!(request >> enteredUser)){  //  make sure all input is entered correctly
		return false;
	}
	if(!(request >> enteredSubject)){
		return false;
	}
	if(!(request >> enteredLength)){
		return false;
	}

	string tempString = "";
	bool firstLoop = true;
	getline(request, tempString); //  skip the "\n" character TODO might not work!
	while(getline(request, tempString)){  //  gets all lines of message
		if(tempString == "")
			break;
		if(!firstLoop)
			enteredMessage += "\n";
		enteredMessage += tempString;
		firstLoop = false;
	}

	Message newMessage;
	newMessage.subject = enteredSubject;
	newMessage.length = atoi(enteredLength.c_str());
	newMessage.message = enteredMessage;

	if(users.find(enteredUser) != users.end()){
		//  if the user already exists just store that data in that map
		//  users.size() inserts at the last index
		users[enteredUser].insert(pair<int, Message>(users[enteredUser].size(), newMessage));
	}else{
		//  if not create a user users entry and put all data into it's first map entry
		map<int, Message> newEntryMap;
		newEntryMap.insert(pair<int, Message>(0, newMessage));		//  inserts message at index 0
		users.insert(pair< string, map<int, Message> >(enteredUser, newEntryMap));
	}

	return true;
}

/**
 * Makes sure the request formatting is correct then retrieves a list of indexs and
 * subjects for all messages of the user requested
 * Makes sure the request user exists as well before proceeding
 * @param stringStream& used by the processRequest function
 * @returns "" if there was an error
 * @returns a correctly formatting list string if successful
 */
string MessagingServer::getMessageListForUser(stringstream &request){
	if(!(request >> enteredUser)){  //  make sure all input is entered correctly
		return "";
	}
	if(users.find(enteredUser) == users.end())
		return "list 0";

	map<int, Message> *tempMap = &users[enteredUser];
	string outputString = "list ";

	//  convert the list length to a string
	ostringstream oss;
	string listLength;
	oss << tempMap->size();  //  adds the list length and a newline to the output string
	listLength = oss.str();
	outputString += listLength;

	for(int i = 0; i < tempMap->size(); i++){
		outputString += "\n";
		oss.str("");  //clear the string stream for a new int to string conversion
		oss << i + 1 << " ";
		outputString += oss.str();
		outputString += (*tempMap)[i].subject;
	}

	return outputString;
}

/**
 * makes sure the correct request formatting is followed and makes sure that
 * the requested user and index exist for message retrieval
 * @param stringStream& used by the processRequest function
 * @returns "" if there was an error
 * @returns a correctly formatting message string if successful
 */
string MessagingServer::getMessage(stringstream &request){
	if(!(request >> enteredUser)){  //  make sure all input is entered correctly
		return "";
	}
	if(!(request >> enteredIndex)){
		return "";
	}
	if(users.find(enteredUser) == users.end())  // TODO this might now work!!! fails on no user
		return "";
	int index = atoi(enteredIndex.c_str());  //  convert string index to int

	if(--index < 0)	//  adjust requested index so it starts at 0 instead of 1 TODO
		return "";

	if(users[enteredUser].find(index) == users[enteredUser].end()) //  fails on no index
		return "";
	Message foundMessage = users[enteredUser].find(index)->second;  // TODO might work? be careful!
	if(foundMessage.subject == "")  //  TODO might not be necessary, if there's not subject it
		return "";					//  doesn't exist... or so the reasoning goes

	string outputString = "message ";
	outputString += foundMessage.subject;
	outputString += " ";

	//  convert the message length to a string
	ostringstream oss;
	oss.str("");
	string listLength;
	oss << foundMessage.length << "\n";  //  adds the message length and a newline to the output string
	outputString += oss.str();

	outputString += foundMessage.message;

	return outputString;
}

/**
 * Goes through every user and erases all elements in their message map
 * Calls delete on the message structs
 */
bool MessagingServer::eraseAllMessages(stringstream &request){
	users.clear();  // FIXME maybe this won't work???? Test it!!!!

	return true;
}

/**
 * Resets all socket input strings to ""
 */
void MessagingServer::resetInput(){
	enteredCommand = enteredUser = enteredIndex = enteredSubject = enteredLength = enteredMessage = "";
}

} /* namespace std */
