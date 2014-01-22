/*
 * MessagingClient.h
 *
 *  Created on: Jan 20, 2014
 *      Author: chris
 */

#ifndef MESSAGINGCLIENT_H_
#define MESSAGINGCLIENT_H_

#include <sstream>
#include "inet-client.h"
#include "MessagingUI.h"

namespace std {

/**
 * Class extends InetClient to provide messaging service through the socket connected to the MessagingServer
 * @author Chris Fortuna
 * @date/ 1/20/2014
 */
class MessagingClient: public InetClient {
public:
	MessagingClient(string host, int port, bool passedDebug);
	virtual ~MessagingClient();

	virtual void run();

private:
	void messagingStart();
	bool getResponse();
	bool finishReadingIfPutRequest(string &request);
	virtual void printServerResponse(string response);

	MessagingUI messagingUI;
	bool debug;
};

} /* namespace std */

#endif /* MESSAGINGCLIENT_H_ */
