/* Copyright (c) Q-stat 2012.
 *
 *	This file is part of PokerServer.
 *
 *	PokerServer is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 * PokerServer is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with PokerServer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef POKERSERVER_H_
#define POKERSERVER_H_

#include <memory>
#include <thread>
#include <set>
#include <vector>
#include "asio.hpp"
#include "ConnectSession.h"

using namespace std;

class ConnectSession;

class PokerServer
{
public:
	static shared_ptr<PokerServer> createPokerServer( const string& address,
			const string& port );

	void start();
	void endSession( shared_ptr<ConnectSession> session );
private:
	PokerServer( shared_ptr<asio::io_service>& io_Service,
			const asio::ip::tcp::endpoint& endPoint, asio::error_code& error );

	void newSession();
	void acceptHandler( shared_ptr<ConnectSession> session,
			const asio::error_code& error );

	shared_ptr<asio::io_service> ioService;
	shared_ptr<asio::ip::tcp::acceptor> acceptor;
	set<shared_ptr<ConnectSession>> sessions;
	vector<shared_ptr<std::thread> > threads;
};

#endif /* POKERSERVER_H_ */
