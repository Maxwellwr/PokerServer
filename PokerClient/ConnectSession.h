/*
 * ConnectSession.h
 *
 *  Created on: 20.12.2012
 *      Author: Alexey
 */

#ifndef CONNECTSESSION_H_
#define CONNECTSESSION_H_

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

	shared_ptr<ServerToClientPackageHdr> sendCommand( const ClientToServerPackageHdr &command );

private:
	ConnectSession();

	void connect( tcp::endpoint endpoint, asio::error_code &errorCode );
	void workerThread();

	asio::io_service ioService_;
	shared_ptr<asio::io_service::work> work_;
	tcp::socket socket_;
	std::mutex mutex_;
	std::condition_variable cond_;
	asio::error_code errorCode_;
	std::thread serviceThread_;
};


#endif /* CONNECTSESSION_H_ */
