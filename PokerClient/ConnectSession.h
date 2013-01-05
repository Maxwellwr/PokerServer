/*
 * ConnectSession.h
 *
 *  Created on: 20.12.2012
 *      Author: Alexey
 */

#ifndef CONNECTSESSION_H_
#define CONNECTSESSION_H_

#include <deque>
#include <thread>
#include <mutex>
#include "asio.hpp"
#include "PackageStructure.hpp"

using asio::ip::tcp;
using namespace std;

#define ATTEMPTS_TO_RETRIEVE_PACKAGE 4

class ConnectSession
{
public:
	static shared_ptr<ConnectSession> createConnectSession(const string address, const string port);
	virtual ~ConnectSession();

	void sendCommand( const ClientToServerPackageHdr &command);
	shared_ptr<ServerToClientPackageHdr> retrievePackage(unsigned int attemptsToRetrieve = ATTEMPTS_TO_RETRIEVE_PACKAGE);

private:
	ConnectSession();

	void connect( tcp::endpoint endpoint, asio::error_code &errorCode );
	void keepAlive();
	void receivePackage();
	void pushToBuffer(shared_ptr<ServerToClientPackageHdr> receivedPackage);
	bool isThreadsActive() const;

	asio::io_service ioService_;
	tcp::socket socket_;
	std::mutex mutex_for_buffer_;
	shared_ptr<std::thread> listenThread_;
	shared_ptr<std::thread> keepAliveThread_;
	asio::error_code errorCode_;
	deque<shared_ptr<ServerToClientPackageHdr>> receievedPackagesBuffer_;
};


#endif /* CONNECTSESSION_H_ */
