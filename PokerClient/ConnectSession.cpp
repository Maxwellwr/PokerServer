/*
 * ConnectSession.cpp
 *
 *  Created on: 20.12.2012
 *      Author: Alexey
 */

#include "ConnectSession.h"

shared_ptr<ConnectSession> ConnectSession::createConnectSession(
		const string address, const string port )
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
		socket_( ioService_ ), work_( ioService_ ), timer_( ioService_ ),
		serviceThread_( &ConnectSession::startWorkerThreads, this )
{
}

void ConnectSession::startWorkerThreads()
{
	try {
//		ioService_.reset();
		if(!listenThread_) {
			listenThread_.reset(new std::thread( [&]() {readPackageHeader(); ioService_.run();} ) );
			listenThread_->detach();
		}
		asio::error_code ec;
		keepAlive( ec );
		ioService_.run();
	} catch (exception &e) {
		cerr << "Error: " << e.what() << "\n";
	}
}

#define POKER_SERVER_TIMEOUT 30

void ConnectSession::keepAlive( const asio::error_code &ec )
{
	auto handler =
			[&](const asio::error_code &ec, const size_t &bytes_transferred) {};
	if (!ec) {
		KeepAlive keep_alive;
		if (socket_.is_open())
			asio::async_write( socket_,
					asio::buffer( &keep_alive, sizeof(keep_alive) ), handler );
		timer_.expires_from_now(
				boost::posix_time::seconds( POKER_SERVER_TIMEOUT ) );
		timer_.async_wait(
				std::bind( &ConnectSession::keepAlive, this, placeholders::_1 ) );
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

void ConnectSession::sendCommand(
		const ClientToServerPackageHdr &command )
{
	if (errorCode_) {
		asio::detail::throw_error( errorCode_, "read package" );
	}
	shared_ptr<ServerToClientPackageHdr> reply;
	std::unique_lock < std::mutex > lk( mutex_ );
	asio::error_code err;
	socket_.write_some(
			asio::buffer( reinterpret_cast<const uint8_t*>( &command ),
					command.bodyLength + sizeof(command) ), err );
	if (!err) {
		cond_.wait( lk );
	} else {
		asio::detail::throw_error( err, "send package" );
	}
}

void ConnectSession::readPackageHeader()
{
//	cout << __PRETTY_FUNCTION__ << endl;
	asio::async_read( socket_,
			asio::buffer( &receivedPackageHdr_, sizeof(receivedPackageHdr_) ),
			bind( &ConnectSession::readPackageHeaderHandler, this,
					placeholders::_1 ) );
}

void ConnectSession::readPackageHeaderHandler( const asio::error_code& error )
{
//	cout << __PRETTY_FUNCTION__ << endl;
	errorCode_ = error;
	if (!error) {
		receivedPackage_.reset(
				reinterpret_cast<ServerToClientPackageHdr*>( new char[sizeof(receivedPackageHdr_)
						+ receivedPackageHdr_.bodyLength] ) );
		*receivedPackage_ = receivedPackageHdr_;
	} else {
		cerr << "Error: " << error.message() << endl;
	}
	if (!errorCode_)
		readPackageBody();
}

void ConnectSession::readPackageBody()
{
//	cout << __PRETTY_FUNCTION__ << endl;
	asio::async_read( socket_,
			asio::buffer(
					reinterpret_cast<char*>( receivedPackage_.get() )
							+ sizeof(receivedPackageHdr_),
					receivedPackage_->bodyLength ),
			std::bind( &ConnectSession::readPackageBodyHandler, this,
					placeholders::_1 ) );
}

void ConnectSession::readPackageBodyHandler( const asio::error_code& error )
{
//	cout << __PRETTY_FUNCTION__ << endl;
	std::lock_guard<std::mutex> lk(mutex_);
	errorCode_ = error;
	if (!error) {
		recievedPackageHandler();
	} else {
		cerr << "Error: " << error.message() << endl;
	}
	cond_.notify_one();
	if (!errorCode_)
		readPackageHeader();
}

void ConnectSession::recievedPackageHandler()
{
	receievedPackagesQueue_.push_back( receivedPackage_ );
}

shared_ptr<ServerToClientPackageHdr> ConnectSession::retrievePackage()
{
	shared_ptr<ServerToClientPackageHdr> reply;
	std::lock_guard<std::mutex> lk(mutex_);
	if (receievedPackagesQueue_.size()){
		reply = receievedPackagesQueue_.front();
		receievedPackagesQueue_.pop_front();
	}
	cond_.notify_one();
	return reply;
}

