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

#ifndef CONNECTSESSION_H_
#define CONNECTSESSION_H_

#include <memory>
#include "asio.hpp"
#include "PackageStructure.hpp"
#include "PokerServer.h"

using namespace std;

class PokerServer;

class ConnectSession : public enable_shared_from_this<ConnectSession>
{
public:
	ConnectSession( PokerServer* server,
			shared_ptr<asio::io_service> io_Service );
	~ConnectSession();

	void start();
	asio::ip::tcp::socket& getSocket();
private:
	void readPackageHeader();
	void readPackageHeaderHandler( const asio::error_code& error );
	void readPackageBody();
	void readPackageBodyHandler( const asio::error_code& error );
	void parsePackage();
	PokerServer* pokerServer;
	shared_ptr<asio::io_service> ioService;
	asio::ip::tcp::socket socket;
	ClientToServerPackageHdr receivedPackageHdr;
	shared_ptr<ClientToServerPackageHdr> receivedPackage;
};

#endif /* CONNECTSESSION_H_ */
