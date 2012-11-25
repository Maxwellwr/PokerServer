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

#include "ConnectSession.h"

ConnectSession::ConnectSession( PokerServer* server,
		shared_ptr<asio::io_service> io_Service ) :
		pokerServer( server ), ioService( io_Service ), socket( *ioService )
{
}

ConnectSession::~ConnectSession()
{
}

asio::ip::tcp::socket& ConnectSession::getSocket()
{
	return socket;
}

void ConnectSession::start()
{
	readPackageHeader();
}

void ConnectSession::readPackageHeader()
{
	asio::async_read( socket, asio::buffer( &receivedPackageHdr, sizeof(receivedPackageHdr) ),
			bind( &ConnectSession::readPackageHeaderHandler, shared_from_this(),
					placeholders::_1 ) );
}

void ConnectSession::readPackageHeaderHandler( const asio::error_code& error )
{
	if (!error) {
		receivedPackage.reset(
				reinterpret_cast<ClientToServerPackageHdr*>( new char[sizeof(receivedPackageHdr)
						+ receivedPackageHdr.bodyLength] ) );
		*receivedPackage = receivedPackageHdr;
		readPackageBody();
	} else {
		cerr << "Error: " << error.message() << endl;
		pokerServer->endSession( this->shared_from_this() );
	}
}

void ConnectSession::readPackageBody()
{
	asio::async_read( socket,
			asio::buffer(
					reinterpret_cast<char*>( receivedPackage.get() + sizeof(receivedPackageHdr) ),
					receivedPackage->bodyLength ),
			std::bind( &ConnectSession::readPackageBodyHandler, shared_from_this(),
					placeholders::_1 ) );
}

void ConnectSession::readPackageBodyHandler( const asio::error_code& error )
{
	if (!error) {
		parsePackage();
		cerr << "Error: " << error.message() << endl;
		pokerServer->endSession( this->shared_from_this() );
	}
}

void ConnectSession::parsePackage()
{

}
