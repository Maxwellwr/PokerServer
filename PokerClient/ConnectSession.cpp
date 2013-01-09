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

#include "ConnectSession.h"

#define KEEP_ALIVE_TIME_INTERVAL 30

shared_ptr<ConnectSession> ConnectSession::createConnectSession(
		const string address, const string port )
{
	asio::io_service io_service;
	shared_ptr<ConnectSession> connectSession;
	asio::ip::tcp::resolver resolver( io_service );
	asio::ip::tcp::resolver::query query( address, port );
	asio::ip::tcp::resolver::iterator endpoint_iterator =
			resolver.resolve( query );
	asio::ip::tcp::resolver::iterator end;
	asio::error_code error( asio::error::host_not_found );
	while (error && endpoint_iterator != end) {
		auto attemptToConnect = new ConnectSession();
		attemptToConnect->connect( *endpoint_iterator++, error );
		if (attemptToConnect->isThreadsActive())
			connectSession.reset( attemptToConnect );
	}
	return connectSession;
}

ConnectSession::ConnectSession() :
		socket_( ioService_ )
{
}

void ConnectSession::keepAlive()
{
	KeepAlivePkg keep_alive;
	asio::error_code error;
	std::chrono::seconds dura( KEEP_ALIVE_TIME_INTERVAL );
	for (;;) {
		std::this_thread::sleep_for( dura );
		socket_.write_some( asio::buffer( &keep_alive, sizeof(keep_alive) ), error );
		if (error == asio::error::eof) {
			return;
		} else if (error) {
			mutex_for_errorCode_.lock();
			lastError_ = error;
			mutex_for_errorCode_.unlock();
			return;
		}
	}
}

ConnectSession::~ConnectSession()
{
	if (socket_.is_open()) {
		socket_.shutdown( tcp::socket::shutdown_both );
		socket_.close();
	}
	if (keepAliveThread_ && keepAliveThread_->joinable())
		keepAliveThread_->join();
	if (listenThread_ && listenThread_->joinable())
		listenThread_->join();
}

void ConnectSession::connect( tcp::endpoint endpoint,
		asio::error_code &errorCode )
{
	socket_.connect( endpoint, errorCode );
	if (errorCode) {
		socket_.close();
		return;
	}
	listenThread_.reset( new std::thread( &ConnectSession::receivePackage, this ) );
	keepAliveThread_.reset( new std::thread( &ConnectSession::keepAlive, this ) );
	stringstream address;
	address << endpoint;
	cout << "Successfully connected to " << address.str() << endl;
}

void ConnectSession::sendCommand( const ClientToServerPackageHdr &command )
{
	if (!isThreadsActive())
		throw lastError_;
	asio::error_code error;
	socket_.write_some( asio::buffer( reinterpret_cast<const uint8_t*>( &command ), command.bodyLength
			+ sizeof(command) ), error );
	if (error) {
		asio::detail::throw_error( error, "send package" );
	}
}

void ConnectSession::receivePackage()
{
	ServerToClientPackageHdr receivedPackageHdr;
	shared_ptr<ServerToClientPackageHdr> receivedPackage;
	asio::error_code error;
	for (;;) {
		socket_.read_some( asio::buffer( &receivedPackageHdr, sizeof(receivedPackageHdr) ), error );
		if (error == asio::error::eof) {
			return;
		} else if (error) {
			break;
		}
		receivedPackage.reset( reinterpret_cast<ServerToClientPackageHdr*>( new char[sizeof(receivedPackageHdr)
				+ receivedPackageHdr.bodyLength] ) );
		*receivedPackage = receivedPackageHdr;
		socket_.read_some( asio::buffer( reinterpret_cast<char*>( receivedPackage.get() )
				+ sizeof(receivedPackageHdr), receivedPackage->bodyLength ), error );
		if (error == asio::error::eof) {
			return;
		} else if (error) {
			break;
		}
		pushToBuffer( receivedPackage );
		signal_.notify_one();
	}
	mutex_for_errorCode_.lock();
	lastError_ = error;
	mutex_for_errorCode_.unlock();
}

void ConnectSession::pushToBuffer(
		shared_ptr<ServerToClientPackageHdr> receivedPackage )
{
	mutex_for_buffer_.lock();
	receievedPackagesBuffer_.push_back( receivedPackage );
	mutex_for_buffer_.unlock();
}

shared_ptr<ServerToClientPackageHdr> ConnectSession::retrievePackage(
		/*unsigned int attemptsToRetrieve*/ )
{
//	std::chrono::milliseconds dura( 100 );
//	for (unsigned int attempt = 0; attempt < attemptsToRetrieve; attempt++) {
//	for(;;) {
//		if (!isThreadsActive())
//			throw lastError_;
//		mutex_for_buffer_.lock();
//		if (receievedPackagesBuffer_.size()) {
//			reply = receievedPackagesBuffer_.front();
//			receievedPackagesBuffer_.pop_front();
//			mutex_for_buffer_.unlock();
//			break;
//		} else {
//			mutex_for_buffer_.unlock();
//			std::mutex mtx;
//			std::unique_lock<std::mutex> lk(mtx);
//			signal_.wait(lk);
////			std::this_thread::sleep_for( dura );
//		}
//	}
	if (!isThreadsActive())
		throw lastError_;
	shared_ptr<ServerToClientPackageHdr> reply;
	mutex_for_buffer_.lock();
	if (!receievedPackagesBuffer_.size()) {
		mutex_for_buffer_.unlock();
		std::mutex mtx;
		std::unique_lock<std::mutex> lk(mtx);
		signal_.wait_for(lk, std::chrono::seconds(1));
		mutex_for_buffer_.lock();
	}
	if (receievedPackagesBuffer_.size()) {
		reply = receievedPackagesBuffer_.front();
		receievedPackagesBuffer_.pop_front();
	}
	mutex_for_buffer_.unlock();

	return reply;
}

bool ConnectSession::isThreadsActive() const
{
	return
			listenThread_ && keepAliveThread_ ?
					listenThread_->joinable() && keepAliveThread_->joinable() : false;
}
