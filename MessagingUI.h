/*
 * MessagingUI.h
 *
 *  Created on: Jan 20, 2014
 *      Author: chris
 */

#ifndef MESSAGINGUI_H_
#define MESSAGINGUI_H_

#include <iostream>
#include <string>

namespace std {

/**
 * This will handle the UI for the messaging service
 * It performs error checking on user input and returns only strings formatted for messaging protocol
 * or returns a null indicating error or Exit
 * @author Chris Fortuna
 * @date 1/20/2014
 */
class MessagingUI {
public:
	/**
	 * Default do nothing constructor
	 */
	MessagingUI():enteredCommand(""), enteredUser(""), enteredSubject(""), enteredIndex(""), enteredMessage(""){}

	/**
	 * Default do nothi destructor
	 */
	virtual ~MessagingUI(){}

	string getInput();
	bool printResponse(string reponse);

private:
	string processSend();
	string processList();
	string processRead();
	string processQuit();
	string processReset();
	void resetInput();

	string enteredCommand;
	string enteredUser;
	string enteredSubject;
	string enteredIndex;
	string enteredMessage;
};

} /* namespace std */

#endif /* MESSAGINGUI_H_ */
