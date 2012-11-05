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

#include "PokerServer.h"

shared_ptr<PokerServer> PokerServer::createPokerServer( const string& address,
		const string& port )
{
	shared_ptr<PokerServer> server;

	auto io_service = make_shared<asio::io_service>();
	RESOLVER resolver( *io_service );
	RESOLVER::query query( address, port );
	RESOLVER::iterator endpoint_iterator = resolver.resolve( query );
	RESOLVER::iterator end;
	asio::error_code error( asio::error::host_not_found );
	while (error && endpoint_iterator != end) {
		auto sample_server =
				new PokerServer( io_service, *endpoint_iterator++, error );
		server.reset( sample_server );
	}
	if (error)
		throw runtime_error( string( "Binding to '" ) + address + string( ":" )
				+ port + string( "' failed" ) );
	return server;
}

PokerServer::PokerServer( shared_ptr<asio::io_service> &ioService,
		const ENDPOINT& endPoint, asio::error_code &error ) :
		m_ioService( ioService )
{
	m_Acceptor.reset( new ACCEPTOR( *m_ioService, endPoint ) );
	error = m_Acceptor->is_open() ? asio::error_code() : asio::error::fault;
}

void PokerServer::start()
{
	newSession();
	while (true) {
		//do something
	}
}

void PokerServer::newSession()
{
	auto newSession = make_shared < ConnectSession > (m_ioService);
	m_Sessions.insert( newSession );
	m_Acceptor->async_accept( newSession->getSocket(), bind( &PokerServer::acceptHandler, this, newSession, placeholders::_1 ) );
	if (m_Sessions.size() > m_Threads.size()) {
		shared_ptr < thread > tmp_thread( new thread( [&]()
		{	m_ioService->run();} ) );
		m_Threads.push_back( tmp_thread );
		tmp_thread->detach();
	}
}

void PokerServer::acceptHandler( shared_ptr<ConnectSession> session,
		const asio::error_code& error )
{
	if (!error) {
		session->reply();
	} else {
		//Print error to stderr
	}
	newSession();
}
