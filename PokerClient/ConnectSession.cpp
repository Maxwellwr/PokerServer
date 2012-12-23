/*
 * ConnectSession.cpp
 *
 *  Created on: 20.12.2012
 *      Author: Alexey
 */

#include "ConnectSession.h"

shared_ptr<ConnectSession> ConnectSession::createConnectSession(const string address, const string port )
{
	asio::io_service io_service;
	shared_ptr<ConnectSession> connectSession;
	asio::ip::tcp::resolver resolver( io_service );
	asio::ip::tcp::resolver::query query( address, port );
	asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(
			query );
	asio::ip::tcp::resolver::iterator end;
	asio::error_code error( asio::error::host_not_found );
	while (error && endpoint_iterator != end) {
		auto attemptToConnect = new ConnectSession();
		attemptToConnect->connect( *endpoint_iterator++, error );
		connectSession.reset( attemptToConnect );
	}
	return connectSession;
}

ConnectSession::ConnectSession() : socket_( ioService_ ), work_(ioService_), timer_(ioService_),
		serviceThread_( &ConnectSession::workerThread, this )
{
}

void ConnectSession::workerThread()
{
	try {
//		ioService_.reset();
		asio::error_code ec;
		keepAlive(ec);
		ioService_.run();
	} catch (exception &e) {
		cerr << "Error: " << e.what() << "\n";
	}
}

#define POKER_SERVER_TIMEOUT 30

void ConnectSession::keepAlive(const asio::error_code &ec)
{
	auto handler = [&](const asio::error_code &ec, const size_t &bytes_transferred) {};
	if ( !ec ) {
		KeepAlive keep_alive;
		if ( socket_.is_open() ) asio::async_write( socket_, asio::buffer( &keep_alive, sizeof(keep_alive) ),
				handler );
		timer_.expires_from_now( boost::posix_time::seconds( POKER_SERVER_TIMEOUT ) );
		timer_.async_wait( std::bind( &ConnectSession::keepAlive, this, placeholders::_1 ) );
	}
}

ConnectSession::~ConnectSession()
{
	ioService_.stop();
	if (serviceThread_.joinable()) {
		serviceThread_.join();
	}
}

void ConnectSession::connect( tcp::endpoint endpoint,
		asio::error_code &errorCode )
{
	auto connect = [&]() {
		socket_.connect( endpoint, errorCode );
		if (errorCode) socket_.close();
		std::lock_guard<std::mutex> lk(mutex_);
		this->cond_.notify_one();
	};
	{
		std::unique_lock < std::mutex > lk( mutex_ );
		ioService_.post( connect );
		cond_.wait( lk );
	}
	if (errorCode) {
		cerr << "Error: " << errorCode.message() << "\n";
		return;
	}

	stringstream address;
	address << endpoint;
	cout << "Successfully connected to " << address.str() << endl;
}

shared_ptr<ServerToClientPackageHdr> ConnectSession::sendCommand(
		const ClientToServerPackageHdr &command )
{
	shared_ptr<ServerToClientPackageHdr> reply;

	auto handler =
			[&](const asio::error_code &ec, const size_t &num_bytes_sent) {
				errorCode_ = ec;
				std::lock_guard<std::mutex> lk(mutex_);
				this->cond_.notify_one();
			};
	{
		std::unique_lock < std::mutex > lk( mutex_ );
		asio::async_write( socket_,
				asio::buffer( reinterpret_cast<const uint8_t*> (&command),
						sizeof(command) + command.bodyLength ), handler );
		cond_.wait( lk );
	}
	if (errorCode_)
		asio::detail::throw_error( errorCode_, "sending command" );
	ServerToClientPackageHdr reply_hdr;
	{
		std::unique_lock < std::mutex > lk( mutex_ );
		asio::async_read( socket_, asio::buffer( &reply_hdr, sizeof(reply_hdr) ),
				handler );
		cond_.wait( lk );
	}
	if (errorCode_)
		asio::detail::throw_error( errorCode_, "reading reply" );
	 reply.reset ( reinterpret_cast<ServerToClientPackageHdr*>(new uint8_t[sizeof(ServerToClientPackageHdr)
					+ reply_hdr.bodyLength]()), default_delete<ServerToClientPackageHdr[]>());
	*reply = reply_hdr;
	{
		std::unique_lock < std::mutex > lk( mutex_ );
		asio::async_read( socket_,
				asio::buffer( reinterpret_cast<uint8_t*>(reply.get()) + sizeof(ServerToClientPackageHdr),
						reply_hdr.bodyLength ), handler );
		cond_.wait( lk );
	}
	if (errorCode_)
		asio::detail::throw_error( errorCode_, "reading reply" );
	if (reply->code == ServerToClientPackageHdr::ERROR_INFO)
	{
		const ErrorFromServer *error = static_cast<const ErrorFromServer*>(reply.get());
		stringstream stream( string( (char*)error->m_errorMessage, error->bodyLength ) );
		char msg[255];
		stream.getline( msg, sizeof(msg), '\0' );
		throw runtime_error( msg );
	}
	return reply;
}

