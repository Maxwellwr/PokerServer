/*
 * PokerClient.cpp
 *
 *  Created on: 16.12.2012
 *      Author: Alexey
 */

#include "PokerClient.h"
//#include <ctime>
#include <iomanip>

PokerClient::PokerClient( shared_ptr<Account> account ) :
		account_( account )
{
}

PokerClient::~PokerClient()
{
}

void PokerClient::connect( const string address, const string port )
{
	connectSession_ = ConnectSession::createConnectSession(address, port );
	shared_ptr<ConnectOk> serverReply(
			static_pointer_cast < ConnectOk
					> (connectSession_->sendCommand(
							Connect( account_->getType(), account_->getName() ) )) );
	for (unsigned int i = 0; i < serverReply->m_toursCount; i++) {
		shared_ptr<Tour> tour( new Tour );
		tour->tourID = serverReply->m_toursIDs[i];
		tours_.insert( pair<uint32_t, shared_ptr<Tour>>( tour->tourID, tour ) );
	}
}

void PokerClient::getTourInfo( uint32_t tourID )
{

	if (tours_.find( tourID ) == tours_.end()) {
		cout << "Error: Tour with id:" << tourID << " doesn't exist" << endl;
		return;
	}
	shared_ptr<TourInfo> serverReply( static_pointer_cast < TourInfo > (connectSession_->sendCommand(GetTourInfo( tourID ) )) );
	if (serverReply && serverReply->code == ServerToClientPackageHdr::TOUR_INFO) {
		auto tour = tours_.find( tourID )->second;
		tour->freePlaces = serverReply->m_freePlaces;
		//XXX: what if StartTime is expired?
		tour->createTime =serverReply->m_creationTime;
		tour->waitTime = serverReply->m_waitTime;
		tour->beginTime = tour->createTime + tour->waitTime;
	} else {
		throw runtime_error("Acquiring a TourInfo has failed.");
	}
}

void PokerClient::selectTour(uint32_t tourID)
{
	if (tours_.find( tourID ) == tours_.end()) {
		cout << "Error: Tour with id:" << tourID << " doesn't exist" << endl;
		return;
	}
	shared_ptr<SelectOk> serverReply( static_pointer_cast < SelectOk > (connectSession_->sendCommand(SelectTour( tourID ) )) );
	if (serverReply && serverReply->code == ServerToClientPackageHdr::SELECT_OK) {
		auto tour = tours_.find(serverReply->m_tourID)->second;
		tour->beginTime = serverReply->m_beginTime;
	} else {
		throw runtime_error("Selection of tour has failed.");
	}
}
uint32_t PokerClient::createTour(uint32_t waitTime, uint32_t maxPlayers)
{
//	shared_ptr<SelectOk> serverReply( static_pointer_cast < SelectOk > (connectSession_->sendCommand(CreateTour(waitTime, maxPlayers) )) );
	auto serverReplyHdr (connectSession_->sendCommand(CreateTour(waitTime, maxPlayers) ));
	if (serverReplyHdr && serverReplyHdr->code == ServerToClientPackageHdr::SELECT_OK) {
		const SelectOk *serverReply = static_cast<SelectOk*> (serverReplyHdr.get());
		shared_ptr<Tour> tour( new Tour );
		tour->tourID = serverReply->m_tourID;
		tour->maxPlayers = maxPlayers;
		tour->waitTime = waitTime;
		tour->beginTime = serverReply->m_beginTime;
		tours_.insert(pair<uint32_t, shared_ptr<Tour>> (tour->tourID, tour));
		return tour->tourID;
	} else {
		throw runtime_error("Creation of tour has failed.");
	}
	return -1;
}

void PokerClient::getPlayersInfo(uint32_t tourID)
{
	if (tours_.find( tourID ) == tours_.end()) {
		cout << "Error: Tour with id:" << tourID << " doesn't exist" << endl;
		return;
	}
	shared_ptr<PlayersInfo> serverReply( static_pointer_cast < PlayersInfo > (connectSession_->sendCommand(GetTourPlayers(tourID) )) );
	if (serverReply && serverReply->code == ServerToClientPackageHdr::PLAYERS_INFO) {
		auto tour = tours_.find(serverReply->m_tourID)->second;
		// Number of bytes occupied by player names in PlayersInfo structure.
		uint32_t occupiedByteForNames = serverReply->bodyLength - 8;
//		cout << "occupiedByteForNames = " << occupiedByteForNames << endl;
		stringstream stream (string (reinterpret_cast<char*>(serverReply->m_playersNames), occupiedByteForNames));
		char name [128];
		while ( stream.getline( name, sizeof(name), '\0' ) ) {
			tour->players.insert(name);
			cout << name << endl;
		}
	} else {
		throw runtime_error("Creation of tour has failed.");
	}
}

void PokerClient::printTourInfo(uint32_t tourID)
{
	if (tours_.find( tourID ) == tours_.end()) {
		cout << "Error: Tour with id:" << tourID << " doesn't exist" << endl;
		return;
	}
	auto tour = tours_.find(tourID)->second;
	stringstream result;
	result << setfill( ' ' ) << setw( 10 ) << "TourID: " << setw( 8 ) << tour->tourID << "\n";
	result << setfill( ' ' ) << setw( 10 ) << "MaxPlayers: " << setw( 8 ) << tour->maxPlayers << "\n";
	result << setfill( ' ' ) << setw( 10 ) << "FreePlaces: " << setw( 8 ) << tour->freePlaces << "\n";
	result << setfill( ' ' ) << setw( 10 ) << "CreateTime: " << setw( 8 ) << tour->createTime << "\n";
	result << setfill( ' ' ) << setw( 10 ) << "WaitTime: " << setw( 8 ) << tour->waitTime << "\n";
	result << setfill( ' ' ) << setw( 10 ) << "BeginTime: " << setw( 8 ) << tour->beginTime << "\n";
	result << setfill( ' ' ) << setw( 10 ) << "Players:\n";
	for (auto it : tour->players) {
		result << it << "\n";
	}
	cout << result.str() << endl;
}

//void PokerClient::printPlayersInfo(uint32_t tourID)
//{
//
//}

void PokerClient::printAllTourID()
{
	cout << "Available tours:" << endl;
	for ( auto it : tours_)
	{
		cout << it.first  << endl;
	}
}
