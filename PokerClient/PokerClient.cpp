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

#include "PokerClient.h"
#include <iomanip>

PokerClient::PokerClient( shared_ptr<Account> account ) :
		account_( account ), turn( false )//, isValid_(false)
{
}

PokerClient::~PokerClient()
{
}

void PokerClient::connect( const string address, const string port )
{
	connectSession_ = ConnectSession::createConnectSession( address, port );
	if (!connectSession_) {
//		isValid_ = false;
		stringstream errorMessage;
		errorMessage << "Can't connect to " << address << ":" << port << endl;
		throw runtime_error( errorMessage.str() );
	}
	connectSession_->sendCommand( ConnectPkg( account_->getType(), account_->getName() ) );
	auto serverReply( connectSession_->retrievePackage() );
	if (serverReply
			&& serverReply->code == ServerToClientPackageHdr::CONNECT_OK) {
		const ConnectOkPkg *reply = static_cast<ConnectOkPkg*>( serverReply.get() );
		for (unsigned int i = 0; i < reply->m_toursCount; i++) {
			shared_ptr<Tour> tour( new Tour );
			tour->tourID = reply->m_toursIDs[i];
			tours_.insert( pair<uint32_t, shared_ptr<Tour>>( tour->tourID, tour ) );
//			isValid_ = true;
		}
	} else {
		throw runtime_error( "Initial acquiring of tourID has failed." );
	}
}

void PokerClient::selectTour( uint32_t tourID )
{
	if (tours_.find( tourID ) == tours_.end()) {
		cout << "Error: Tour with id:" << tourID << " doesn't exist" << endl;
		return;
	}
	connectSession_->sendCommand( SelectTourPkg( tourID ) );
	auto serverReply( connectSession_->retrievePackage() );
	cout << "selectTour reply: " << serverReply->code << endl;
	if (serverReply
			&& serverReply->code == ServerToClientPackageHdr::SELECT_OK) {
		if (!handleThread)
			handleThread.reset( new std::thread( std::bind( &PokerClient::handleGamePackageData, this ) ) );
		const SelectOkPkg *reply = static_cast<SelectOkPkg*>( serverReply.get() );
		auto tour = tours_.find( reply->m_tourID )->second;
		tour->beginTime = reply->m_beginTime;
	} else {
		throw runtime_error( "Selection of tour has failed." );
	}
}
uint32_t PokerClient::createTour( uint32_t waitTime, uint32_t maxPlayers )
{
	connectSession_->sendCommand( CreateTourPkg( waitTime, maxPlayers ) );
	auto serverReply( connectSession_->retrievePackage() );
	if (serverReply)
		cout << "Package code: " << serverReply->code << endl;
	if (serverReply
			&& serverReply->code == ServerToClientPackageHdr::SELECT_OK) {
		if (!handleThread)
			handleThread.reset( new std::thread( std::bind( &PokerClient::handleGamePackageData, this ) ) );
		const SelectOkPkg *reply = static_cast<SelectOkPkg*>( serverReply.get() );
		shared_ptr<Tour> tour( new Tour );
		tour->tourID = reply->m_tourID;
		tour->maxPlayers = maxPlayers;
		tour->waitTime = waitTime;
		tour->beginTime = reply->m_beginTime;
		tours_.insert( pair<uint32_t, shared_ptr<Tour>>( tour->tourID, tour ) );
		return tour->tourID;
	} else {
		throw runtime_error( "Creation of tour has failed." );
	}
	return -1;
}

void PokerClient::getTourInfo( uint32_t tourID )
{

//	if (tours_.find( tourID ) == tours_.end()) {
//		cout << "Error: Tour with id:" << tourID << " doesn't exist" << endl;
//		return;
//	}
	connectSession_->sendCommand( GetTourInfoPkg( tourID ) );
	auto serverReply( connectSession_->retrievePackage() );
	if (serverReply
			&& serverReply->code == ServerToClientPackageHdr::TOUR_INFO) {
		const TourInfoPkg *reply = static_cast<TourInfoPkg*>( serverReply.get() );
		auto tour = tours_.find( tourID )->second;
		tour->freePlaces = reply->m_freePlaces;
		tour->createTime = reply->m_creationTime;
		tour->waitTime = reply->m_waitTime;
		//XXX: what if beginTime is expired?
		tour->beginTime = tour->createTime + tour->waitTime;
	} else {
		throw runtime_error( "Acquiring a TourInfo has failed." );
	}
}

void PokerClient::getPlayersInfo( uint32_t tourID )
{
//	if (tours_.find( tourID ) == tours_.end()) {
//		cout << "Error: Tour with id:" << tourID << " doesn't exist" << endl;
//		return;
//	}
	connectSession_->sendCommand( GetTourPlayersPkg( tourID ) );
	auto serverReply( connectSession_->retrievePackage() );
	if (serverReply
			&& serverReply->code == ServerToClientPackageHdr::PLAYERS_INFO) {
		const PlayersInfoPkg *reply = static_cast<PlayersInfoPkg*>( serverReply.get() );
		auto tour = tours_.find( reply->m_tourID )->second;
		// Number of bytes occupied by player names in PlayersInfo structure.
		uint32_t occupiedByteForNames = reply->bodyLength - 8;
		stringstream stream( string( reinterpret_cast<const char*>( reply->m_playersNames ), occupiedByteForNames ) );
		char name[32];
		while (stream.getline( name, sizeof(name), '\0' )) {
			tour->players.insert( name );
		}
	} else {
		throw runtime_error( "Acquiring of players information has failed." );
	}
}



void PokerClient::printTourInfo( uint32_t tourID )
{
	if (tours_.find( tourID ) == tours_.end()) {
		cout << "Error: Tour with id:" << tourID << " doesn't exist" << endl;
		return;
	}
	getTourInfo(tourID);
	getPlayersInfo(tourID);
	auto tour = tours_.find( tourID )->second;
	stringstream result;
	result << setfill( ' ' ) << setw( 10 ) << "TourID: " << setw( 8 )
			<< tour->tourID << "\n";
	result << setfill( ' ' ) << setw( 10 ) << "MaxPlayers: " << setw( 8 )
			<< tour->maxPlayers << "\n";
	result << setfill( ' ' ) << setw( 10 ) << "FreePlaces: " << setw( 8 )
			<< tour->freePlaces << "\n";
	result << setfill( ' ' ) << setw( 10 ) << "CreateTime: " << setw( 8 )
			<< tour->createTime << "\n";
	result << setfill( ' ' ) << setw( 10 ) << "WaitTime: " << setw( 8 )
			<< tour->waitTime << "\n";
	result << setfill( ' ' ) << setw( 10 ) << "BeginTime: " << setw( 8 )
			<< tour->beginTime << "\n";
	result << setfill( ' ' ) << setw( 10 ) << "Players:\n";
	for (auto it : tour->players) {
		result << it << "\n";
	}
	cout << result.str() << endl;
}

void PokerClient::printAllTourID()
{
	getTours();
	cout << "Available tours:" << endl;
	for (auto it : tours_) {
		cout << it.first << endl;
	}
}

void PokerClient::getTours()
{
	connectSession_->sendCommand( GetToursPkg() );
	auto serverReply( connectSession_->retrievePackage() );

	if (serverReply)
		cout << "Package Code: " << serverReply->code << endl;
	if (serverReply
			&& serverReply->code == ServerToClientPackageHdr::CONNECT_OK) {
		const ConnectOkPkg *reply = static_cast<ConnectOkPkg*>( serverReply.get() );
		for (unsigned int i = 0; i < reply->m_toursCount; i++) {
			shared_ptr<Tour> tour( new Tour );
			tour->tourID = reply->m_toursIDs[i];
			tours_.insert( pair<uint32_t, shared_ptr<Tour>>( tour->tourID, tour ) );
		}
	} else {
		throw runtime_error( "Acquiring of tourID has failed." );
	}
}

void PokerClient::call()
{
	if (isTurn()) {
		connectSession_->sendCommand( GameTableActionPkg( GameTableActionPkg::CALL ) );
		if (!handleThread)
			handleThread.reset( new std::thread( std::bind( &PokerClient::handleGamePackageData, this ) ) );
//		auto serverReply(connectSession_->retrievePackage());
//		shared_ptr<ServerToClientPackageHdr> serverReply;
//		do {
//			serverReply = connectSession_->retrievePackage();
//			if (serverReply)
//				handleGamePackageData(serverReply);
//		} while (serverReply && serverReply->code != ServerToClientPackageHdr::GAME_TABLE_INFO);

//		if (serverReplyHdr && serverReplyHdr->code == ServerToClientPackageHdr::GAME_TABLE_INFO) {
//			printTableInfo(static_pointer_cast<GameTableInfo>(serverReplyHdr));
//			endTurn();
//		}
	} else {
		cout << "Please wait for your turn." << endl;
	}
}

void PokerClient::rise( uint32_t money )
{
	if (isTurn()) {
		connectSession_->sendCommand( GameTableActionPkg( GameTableActionPkg::RISE, money ) );
		if (!handleThread)
			handleThread.reset( new std::thread( std::bind( &PokerClient::handleGamePackageData, this ) ) );
//		auto serverReply(connectSession_->retrievePackage());
//		if (serverReplyHdr && serverReplyHdr->code == ServerToClientPackageHdr::GAME_TABLE_INFO) {
//			printTableInfo(static_pointer_cast<GameTableInfo>(serverReplyHdr));
//			endTurn();
//		}
	} else {
		cout << "Please wait for your turn." << endl;
	}
}

void PokerClient::fald()
{
	if (isTurn()) {
		if (!handleThread)
			handleThread.reset( new std::thread( std::bind( &PokerClient::handleGamePackageData, this ) ) );
		connectSession_->sendCommand( GameTableActionPkg( GameTableActionPkg::FALD ) );
//		auto serverReply(connectSession_->retrievePackage());
//		if (serverReplyHdr && serverReplyHdr->code == ServerToClientPackageHdr::GAME_TABLE_INFO) {
//			printTableInfo(static_pointer_cast<GameTableInfo>(serverReplyHdr));
//			endTurn();
//		}
	} else {
		cout << "Please wait for your turn." << endl;
	}
}

//TODO: create a thread handler for PokerClient::handleGamePackageData
void PokerClient::handleGamePackageData(
/*shared_ptr<ServerToClientPackageHdr> reply*/)
{
	shared_ptr<ServerToClientPackageHdr> reply;
	do {
//		unsigned int attempts = 8;
		reply = connectSession_->retrievePackage();
		if (reply) {
			cout << "handleGamePackageData Code: " << reply->code << endl;
			switch (reply->code)
			{
			case ServerToClientPackageHdr::GAME_TABLE_INFO:
				{
					endTurn();
					cout << "GAME_TABLE_INFO" << endl;
					printTableInfo (static_pointer_cast<GameTableInfoPkg>( reply ));
					cout << "\n" << endl;
					break;				}
				case ServerToClientPackageHdr::TOUR_START:
				{
					cout << "TOUR_START" << endl;
					break;
				}
				case ServerToClientPackageHdr::START_GAME:
				{
					cout << "START_GAME" << endl;
					break;
				}
				case ServerToClientPackageHdr::GAME_TABLE_ACTION_INVITE:
				{
					cout << "GAME_TABLE_ACTION_INVITE" << endl;
					cout << "It's your turn." << endl;
					turn = true;
					break;
				}
				case ServerToClientPackageHdr::END_GAME_RESULT:
				{
					cout << "END_GAME_RESULT" << endl;
					cout << "The winner is " << static_cast<EndGameResultPkg*>(reply.get())->m_winner << endl;
					break;
				}
				case ServerToClientPackageHdr::ERROR_INFO:
				{
					cout << "ERROR_INFO" << endl;
					break;
				}
				default:
				cout << "UNKNOW CODE" << endl;
				break;
			}
		}
	}while(!reply || reply->code != ServerToClientPackageHdr::END_GAME);
}

bool PokerClient::isTurn() const
{
	return turn;
}

void PokerClient::endTurn()
{
	turn = false;
}

void PokerClient::printTableInfo( shared_ptr<GameTableInfoPkg> gameTableInfo )
{
	cout << "Card on table: " << endl;
	for (int i = 0; i < CARDS_ON_TABLE; i++) {
		printCard( gameTableInfo->m_cardOnTable[i] );
	}
	const PlayerInfo* playerInfo = gameTableInfo->getPlayerInfoPntr();
	const BankInfo* bankInfo = gameTableInfo->getBankInfoPntr();
	for (; (char*) playerInfo != (char*) bankInfo; playerInfo++) {
		const PlayerInfo& pinf = *playerInfo;
		cout << "m_playersCount: " << gameTableInfo->m_playersCount << endl;
		cout << "name_: " << pinf.name_ << endl;
		cout << "stack_: " << pinf.stack_ << endl;
		cout << "lastAction_: " << (uint32_t) pinf.lastAction_ << endl;
		cout << "firstCard_: ";
		printCard( pinf.pocketCards_.firstCard_ );
		cout << "secondCard_: ";
		printCard( pinf.pocketCards_.secondCard_ );
	}
//	for (int i = 0; i < gameTableInfo->m_playersCount; i++) {
//		const PlayerInfo& pinf = gameTableInfo->m_playersInfo[i];
//		cout << "m_playersCount: " << gameTableInfo->m_playersCount << endl;
//		cout << "name_: " << pinf.name_ << endl;
//		cout << "stack_: " << pinf.stack_ << endl;
//		cout << "lastAction_: " << (uint32_t) pinf.lastAction_ << endl;
//		cout << "firstCard_: " ;
//		printCard(pinf.pocketCards_.firstCard_);
//		cout << "secondCard_: ";
//		printCard(pinf.pocketCards_.secondCard_);
//	}
}

void PokerClient::printCard( const GameCard& card ) const
{
	if (!card.getRank()) {
		cout << "None" << endl;
		return;
	}
	string str;
	switch (card.getSuit())
	{
	case GameCard::Clubs:
		str = "Clubs";
		break;
	case GameCard::Diamond:
		str = "Diamond";
		break;
	case GameCard::Hearts:
		str = "Hearts";
		break;
	case GameCard::Spades:
		str = "Spades";
		break;
	default:
		str = "wrong number";
		break;
	}
	cout << " Suit: " << str;
	switch (card.getRank())
	{
	case GameCard::none:
		str = "none";
		break;
	case GameCard::Ace:
		str = "Ace";
		break;
	case GameCard::Two:
		str = "Two";
		break;
	case GameCard::Three:
		str = "Three";
		break;
	case GameCard::Four:
		str = "Four";
		break;
	case GameCard::Five:
		str = "Five";
		break;
	case GameCard::Six:
		str = "Six";
		break;
	case GameCard::Seven:
		str = "Seven";
		break;
	case GameCard::Eight:
		str = "Eight";
		break;
	case GameCard::Nine:
		str = "Nine";
		break;
	case GameCard::Ten:
		str = "Ten";
		break;
	case GameCard::Jack:
		str = "Jack";
		break;
	case GameCard::Queen:
		str = "Queen";
		break;
	case GameCard::King:
		str = "King";
		break;
	default:
		str = "wrong number";
		break;
	}
	cout << " Rank: " << str << endl;
}
