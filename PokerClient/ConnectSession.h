/* Copyright (c) 2012, 2013 Q-stat.
 *
 *	This file is part of PokerClient.
 *
 *	PokerClient is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 * PokerClient is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with PokerServer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONNECTSESSION_H_
#define CONNECTSESSION_H_

#include <deque>
#include <thread>
#include <condition_variable>
#include <mutex>
#include "asio.hpp"
#include "PackageStructure.hpp"

using asio::ip::tcp;
using namespace std;

//#define ATTEMPTS_TO_RETRIEVE_PACKAGE 4

class ConnectSession
{
public:
	static shared_ptr<ConnectSession> createConnectSession(const string address, const string port);
	virtual ~ConnectSession();

	void sendCommand( const ClientToServerPackageHdr &command);
	shared_ptr<ServerToClientPackageHdr> retrievePackage(/*unsigned int attemptsToRetrieve = ATTEMPTS_TO_RETRIEVE_PACKAGE*/);

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
	std::mutex mutex_for_errorCode_;
	shared_ptr<std::thread> listenThread_;
	shared_ptr<std::thread> keepAliveThread_;
	condition_variable signal_;
	asio::error_code lastError_;
	deque<shared_ptr<ServerToClientPackageHdr>> receievedPackagesBuffer_;
};


#endif /* CONNECTSESSION_H_ */
