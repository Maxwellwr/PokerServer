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
#include <condition_variable>
#include "asio.hpp"
#include "PackageStructure.hpp"

using asio::ip::tcp;
using namespace std;

class ConnectSession
{
public:
	static shared_ptr<ConnectSession> createConnectSession(const string address, const string port);
	virtual ~ConnectSession();

	void sendCommand( const ClientToServerPackageHdr &command);
	shared_ptr<ServerToClientPackageHdr> retrievePackage();

private:
	ConnectSession();

	void connect( tcp::endpoint endpoint, asio::error_code &errorCode );
	void startWorkerThreads();
	void keepAlive(const asio::error_code &ec);
	void recievedPackageHandler();

	void readPackageHeader();
	void readPackageHeaderHandler( const asio::error_code& error );
	void readPackageBody();
	void readPackageBodyHandler( const asio::error_code& error );

	asio::io_service ioService_;
	tcp::socket socket_;
	std::mutex mutex_;
	asio::io_service::work work_;
	asio::deadline_timer timer_;
	std::condition_variable cond_;
	std::thread serviceThread_;
	shared_ptr<std::thread> listenThread_;
	asio::error_code errorCode_;
	deque<shared_ptr<ServerToClientPackageHdr>> receievedPackagesQueue_;
	ServerToClientPackageHdr receivedPackageHdr_;
	shared_ptr<ServerToClientPackageHdr> receivedPackage_;
};


#endif /* CONNECTSESSION_H_ */
