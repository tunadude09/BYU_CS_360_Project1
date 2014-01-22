/*
 * MessagingUI.cpp
 *
 *  Created on: Jan 20, 2014
 *      Author: chris
 */

#include "MessagingUI.h"
#include <sstream>
#include <string>

namespace std {

/**
 * Gets user input from the commandline, looping until the format is correct or the user quits
 * Calls process send/list/read/quit to correctly format the string depending on the request made
 * @returns string formattedMessage or  if the user chooses to exit
 */
string MessagingUI::getInput(){
	string messageString = "";
	bool correctInput = false;

	while(!correctInput){
		cout << "% ";
		cin >> enteredCommand;
		if(enteredCommand == "send"){
			messageString = processSend();
			if(messageString != "")
				correctInput = true;
			else
				cout << "error INCORRECT COMMANDLINE FORMATTING\n";
		}else if(enteredCommand == "list"){
			messageString = processList();
			if(messageString != "")
				correctInput = true;
			else
				cout << "error INCORRECT COMMANDLINE FORMATTING\n";
		}else if(enteredCommand == "read"){
			messageString = processRead();
			if(messageString != "")
				correctInput = true;
			else
				cout << "error INCORRECT COMMANDLINE FORMATTING\n";
		}else if(enteredCommand == "quit"){  // if quit is followed by other characters
			messageString = processQuit();
			if(messageString == "")
				correctInput = true;
			else
				cout << "error INCORRECT COMMANDLINE FORMATTING\n";
		}else if(enteredCommand == "reset"){  // if quit is followed by other characters
			messageString = processReset();
			if(messageString != "")
				correctInput = true;
			else
				cout << "error INCORRECT COMMANDLINE FORMATTING\n";
		}else{
			cout << "error INCORRECT COMMANDLINE FORMATTING\n";
			getline(cin, messageString);
		}
	}


	resetInput();
	return messageString;
}

/**
 * Checks for correct formatting in the rest of the user's send input request
 * It then changes the formatting to follow messsaging protocol
 * @return correct Messaging Protocol send string or "" if incorrect formatting is given
 */
string MessagingUI::processSend(){
	string tempInput = "";
	getline(cin, tempInput);
	stringstream inputSs(tempInput);

	//  check to make sure user follows correct formatting
	if(!(inputSs >> enteredUser)){
		return "";
	}
	if(!(inputSs >> enteredSubject)){
		return "";
	}
	if(inputSs >> tempInput)
		return "";

	cout << "- Type your message. End with a blank line -\n";
	while(getline(cin, tempInput)){
		if(tempInput == "")
			break;
		enteredMessage += tempInput;
		enteredMessage += "\n";  // FIXME I should find a better way to send messages with \n chars
	}

	//  convert the message length to a string
	ostringstream oss;
	oss.str("");
	string messageLength;
	oss << enteredMessage.length();
	messageLength = oss.str();

	return "put " + enteredUser + " " + enteredSubject + " " + messageLength
			+ "\n" + enteredMessage;
}

/**
 * Checks for correct formatting in the rest of the user's list input request
 * It then changes the formatting to follow messsaging protocol
 * @return correct Messaging Protocol list string or "" if incorrect formatting is given
 */
string MessagingUI::processList(){
	string tempInput = "";
	getline(cin, tempInput);
	stringstream inputSs(tempInput);

	//  check to make sure user follows correct formatting
	if(!(inputSs >> enteredUser)){
		return "";
	}
	if(inputSs >> tempInput)
		return "";

	return "list " + enteredUser;
}

/**
 * Checks for correct formatting in the rest of the user's read input request
 * It then changes the formatting to follow messsaging protocol
 * @return correct Messaging Protocol read string or "" if incorrect formatting is given
 */
string MessagingUI::processRead(){
	string tempInput = "";
	getline(cin, tempInput);
	stringstream inputSs(tempInput);

	//  check to make sure user follows correct formatting
	if(!(inputSs >> enteredUser)){
		return "";
	}
	if(!(inputSs >> enteredIndex)){
		return "";
	}
	if(inputSs >> tempInput)
		return "";

	return "get " + enteredUser + " " + enteredIndex;
}

/**
 * Makes sure only quit is called on the commandline
 * checks for correct quit formatting
 * @return fail if anything else follows quit on the commandline
 * @return returns "" to indicated correct formatting if quit is alone
 */
string MessagingUI::processQuit(){
	string tempInput = "";
	getline(cin, tempInput);
	stringstream inputSs(tempInput);

	//  check to make sure user follows correct formatting
	if(inputSs >> tempInput){
		return "fail";
	}

	return "";
}

/**
 * Makes sure only reset is put on the commandline
 * Checks for correct formatting to reset the server
 * @return fail if anything else follows quit on the commandline
 * @return returns "" to indicated correct formatting if quit is alone
 */
string MessagingUI::processReset(){
	string tempInput = "";
	getline(cin, tempInput);
	stringstream inputSs(tempInput);

	//  check to make sure user follows correct formatting
	if(inputSs >> tempInput){
		return "";
	}

	return "reset";
}

/**
 * Deletes all data stored in the MessagingUI input so far
 */
void MessagingUI::resetInput(){
	enteredCommand = enteredUser = enteredSubject = enteredIndex = enteredMessage = "";
}

/**
 * This prints reponses recieved from the server in the format correct
 * for the user
 * @param string response recieved from server
 * @return bool indicating success
 */  //  FIXME I need to watch all the << endl entries
bool MessagingUI::printResponse(string response){
	resetInput();  //  TODO this might be overkill
	stringstream requestStream(response);
	int lineCount = 0;  //  makes sure \n isn't printed on the first line of list and message

	requestStream >> enteredCommand;
	if(enteredCommand == "OK"){
		// TODO this will print eventually if -d enabled
	}else if(enteredCommand == "error"){
		cout << response;

	}else if(enteredCommand == "list"){
		getline(requestStream, enteredIndex);  // skip the returned length
		while(getline(requestStream, enteredIndex)){  //  use enteredIndex as temp to store next line
			enteredMessage += enteredIndex;
			enteredMessage += "\n";
		}
		cout << enteredMessage;  //  TODO might need a \n here?

	}else if(enteredCommand == "message"){
		requestStream >> enteredSubject;
		getline(requestStream, enteredIndex);  //  use enteredIndex as temp to skip length

		while(getline(requestStream, enteredIndex)){  //  use enteredIndex as temp to store next line
			if(lineCount)
				enteredMessage += "\n";
			enteredMessage += enteredIndex;  //  print the message as sent
			lineCount++;
		}
		cout << enteredSubject << endl << enteredMessage << "\n";

	}else{
		cout << "error UNRECOGNIZED SERVER RESPONSE\n";
	}


	resetInput();  //  TODO this might be overkill
	return false;
}

} /* namespace std */


//system.("clear") to clear the terminal
