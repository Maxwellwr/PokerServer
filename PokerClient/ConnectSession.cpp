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

ConnectSession::ConnectSession() :
		work_(new asio::io_service::work (ioService_)), socket_( ioService_ ),
		serviceThread_( &ConnectSession::workerThread, this )
{
}

void ConnectSession::workerThread()
{
	try {
		ioService_.reset();
		ioService_.run();
	} catch (exception &e) {
		cerr << "Error: " << e.what() << "\n";
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
	auto handler =
			[&](const asio::error_code &ec, const size_t &num_bytes_sent) {
				errorCode_ = ec;
				std::lock_guard<std::mutex> lk(mutex_);
				this->cond_.notify_one();
			};
	{
		std::unique_lock < std::mutex > lk( mutex_ );
		asio::async_write( socket_,
				asio::buffer( (uint8_t*) &command,
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
//	cout << "Package type:" << hex << (ServerToClientPackageHdr::ServCommandCode)reply_hdr.code << endl;
//	cout << "Package body length:" << reply_hdr.bodyLength << endl;
	auto reply =
			(ServerToClientPackageHdr*) new uint8_t[sizeof(ServerToClientPackageHdr)
					+ reply_hdr.bodyLength];
	*reply = reply_hdr;
	{
		std::unique_lock < std::mutex > lk( mutex_ );
		asio::async_read( socket_,
				asio::buffer( (uint8_t*) reply + sizeof(ServerToClientPackageHdr),
						reply_hdr.bodyLength ), handler );
		cond_.wait( lk );
	}
	if (errorCode_)
		asio::detail::throw_error( errorCode_, "reading reply" );
//	cout << "End of package acquiring." << endl;
	return shared_ptr < ServerToClientPackageHdr
			> (reply, std::default_delete<ServerToClientPackageHdr[]>());
}

