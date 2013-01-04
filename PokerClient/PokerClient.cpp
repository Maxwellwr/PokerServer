/*
 * PokerClient.cpp
 *
 *  Created on: 16.12.2012
 *      Author: Alexey
 */

#include "PokerClient.h"
//#include <ctime>
#include <iomanip>
#include <unistd.h>

PokerClient::PokerClient(shared_ptr<Account> account) :
		account_(account), turn(false) {
}

PokerClient::~PokerClient() {
}

void PokerClient::connect(const string address, const string port) {
	connectSession_ = ConnectSession::createConnectSession(address, port);
	if (!connectSession_) {
		stringstream errorMessage;
		errorMessage << "Can't connect to " << address << ":" << port << endl;
		throw runtime_error(errorMessage.str());
	}
	connectSession_->sendCommand(	Connect(account_->getType(), account_->getName()));
	auto serverReply(connectSession_->retrievePackage());
	if (serverReply && serverReply->code == ServerToClientPackageHdr::CONNECT_OK) {
		const ConnectOk *reply = static_cast<ConnectOk*>(serverReply.get());
		for (unsigned int i = 0; i < reply->m_toursCount; i++) {
			shared_ptr<Tour> tour(new Tour);
			tour->tourID = reply->m_toursIDs[i];
			tours_.insert(pair<uint32_t, shared_ptr<Tour>>(tour->tourID, tour));
		}
	} else {
		throw runtime_error("Initial acquiring of tourID has failed.");
	}
}

void PokerClient::getTourInfo(uint32_t tourID) {

	if (tours_.find(tourID) == tours_.end()) {
		cout << "Error: Tour with id:" << tourID << " doesn't exist" << endl;
		return;
	}
	connectSession_->sendCommand(GetTourInfo(tourID));
	auto serverReply(connectSession_->retrievePackage());
	if (serverReply
			&& serverReply->code == ServerToClientPackageHdr::TOUR_INFO) {
		const TourInfo *reply = static_cast<TourInfo*>(serverReply.get());
		auto tour = tours_.find(tourID)->second;
		tour->freePlaces = reply->m_freePlaces;
		//XXX: what if StartTime is expired?
		tour->createTime = reply->m_creationTime;
		tour->waitTime = reply->m_waitTime;
		tour->beginTime = tour->createTime + tour->waitTime;
	} else {
		throw runtime_error("Acquiring a TourInfo has failed.");
	}
}

void PokerClient::selectTour(uint32_t tourID) {
	if (tours_.find(tourID) == tours_.end()) {
		cout << "Error: Tour with id:" << tourID << " doesn't exist" << endl;
		return;
	}
	connectSession_->sendCommand(SelectTour(tourID));
	auto serverReply(connectSession_->retrievePackage());
	cout << "selectTour reply: " << serverReply->code << endl;
	if (serverReply
			&& serverReply->code == ServerToClientPackageHdr::SELECT_OK) {
		if (!handleThread)
			handleThread.reset(
					new std::thread(
							std::bind(&PokerClient::handleGamePackageData,
									this)));
		const SelectOk *reply = static_cast<SelectOk*>(serverReply.get());
		auto tour = tours_.find(reply->m_tourID)->second;
		tour->beginTime = reply->m_beginTime;
	} else {
		throw runtime_error("Selection of tour has failed.");
	}
}
uint32_t PokerClient::createTour(uint32_t waitTime, uint32_t maxPlayers) {
	connectSession_->sendCommand(CreateTour(waitTime, maxPlayers));
	auto serverReply(connectSession_->retrievePackage());
	if (serverReply)
	cout << "Package code: " << serverReply->code << endl;
	if (serverReply
			&& serverReply->code == ServerToClientPackageHdr::SELECT_OK) {
		if (!handleThread)
			handleThread.reset(
					new std::thread(
							std::bind(&PokerClient::handleGamePackageData,
									this)));
		const SelectOk *reply = static_cast<SelectOk*>(serverReply.get());
		shared_ptr<Tour> tour(new Tour);
		tour->tourID = reply->m_tourID;
		tour->maxPlayers = maxPlayers;
		tour->waitTime = waitTime;
		tour->beginTime = reply->m_beginTime;
		tours_.insert(pair<uint32_t, shared_ptr<Tour>>(tour->tourID, tour));
		return tour->tourID;
	} else {
		throw runtime_error("Creation of tour has failed.");
	}
	return -1;
}

void PokerClient::getPlayersInfo(uint32_t tourID) {
	if (tours_.find(tourID) == tours_.end()) {
		cout << "Error: Tour with id:" << tourID << " doesn't exist" << endl;
		return;
	}
	connectSession_->sendCommand(GetTourPlayers(tourID));
	auto serverReply(connectSession_->retrievePackage());
	if (serverReply
			&& serverReply->code == ServerToClientPackageHdr::PLAYERS_INFO) {
		const PlayersInfo *reply = static_cast<PlayersInfo*>(serverReply.get());
		auto tour = tours_.find(reply->m_tourID)->second;
		// Number of bytes occupied by player names in PlayersInfo structure.
		uint32_t occupiedByteForNames = reply->bodyLength - 8;
		stringstream stream(
				string(reinterpret_cast<const char*>(reply->m_playersNames),
						occupiedByteForNames));
		char name[128];
		while (stream.getline(name, sizeof(name), '\0')) {
			tour->players.insert(name);
			cout << name << endl;
		}
	} else {
		throw runtime_error("Acquiring of players information has failed.");
	}
}

void PokerClient::printTourInfo(uint32_t tourID) {
	if (tours_.find(tourID) == tours_.end()) {
		cout << "Error: Tour with id:" << tourID << " doesn't exist" << endl;
		return;
	}
	auto tour = tours_.find(tourID)->second;
	stringstream result;
	result << setfill(' ') << setw(10) << "TourID: " << setw(8) << tour->tourID
			<< "\n";
	result << setfill(' ') << setw(10) << "MaxPlayers: " << setw(8)
			<< tour->maxPlayers << "\n";
	result << setfill(' ') << setw(10) << "FreePlaces: " << setw(8)
			<< tour->freePlaces << "\n";
	result << setfill(' ') << setw(10) << "CreateTime: " << setw(8)
			<< tour->createTime << "\n";
	result << setfill(' ') << setw(10) << "WaitTime: " << setw(8)
			<< tour->waitTime << "\n";
	result << setfill(' ') << setw(10) << "BeginTime: " << setw(8)
			<< tour->beginTime << "\n";
	result << setfill(' ') << setw(10) << "Players:\n";
	for (auto it : tour->players) {
		result << it << "\n";
	}
	cout << result.str() << endl;
}

void PokerClient::printAllTourID() {
	getTours();
	cout << "Available tours:" << endl;
	for (auto it : tours_) {
		cout << it.first << endl;
	}
}

void PokerClient::getTours() {
	connectSession_->sendCommand(GetTours());
	auto serverReply(connectSession_->retrievePackage());

	if (serverReply)
		cout << "Package Code: " << serverReply->code << endl;
	if (serverReply
			&& serverReply->code == ServerToClientPackageHdr::CONNECT_OK) {
		const ConnectOk *reply = static_cast<ConnectOk*>(serverReply.get());
		for (unsigned int i = 0; i < reply->m_toursCount; i++) {
			shared_ptr<Tour> tour(new Tour);
			tour->tourID = reply->m_toursIDs[i];
			tours_.insert(pair<uint32_t, shared_ptr<Tour>>(tour->tourID, tour));
		}
	} else {
		throw runtime_error("Acquiring of tourID has failed.");
	}
}

void PokerClient::call() {
	if (isTurn()) {
		connectSession_->sendCommand(GameTableAction(GameTableAction::CALL));
		if (!handleThread)
			handleThread.reset(
					new std::thread(
							std::bind(&PokerClient::handleGamePackageData,
									this)));
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

void PokerClient::rise(uint32_t money) {
	if (isTurn()) {
		connectSession_->sendCommand(
				GameTableAction(GameTableAction::RISE, money));
		if (!handleThread)
			handleThread.reset(
					new std::thread(
							std::bind(&PokerClient::handleGamePackageData,
									this)));
//		auto serverReply(connectSession_->retrievePackage());
//		if (serverReplyHdr && serverReplyHdr->code == ServerToClientPackageHdr::GAME_TABLE_INFO) {
//			printTableInfo(static_pointer_cast<GameTableInfo>(serverReplyHdr));
//			endTurn();
//		}
	} else {
		cout << "Please wait for your turn." << endl;
	}
}

void PokerClient::fald() {
	if (isTurn()) {
		if (!handleThread)
			handleThread.reset(
					new std::thread(
							std::bind(&PokerClient::handleGamePackageData,
									this)));
		connectSession_->sendCommand(GameTableAction(GameTableAction::FALD));
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
/*shared_ptr<ServerToClientPackageHdr> reply*/) {
	shared_ptr<ServerToClientPackageHdr> reply;
	do {
		reply = connectSession_->retrievePackage();
		if (reply) {
			cacheReplyPckage.push_back(reply);
//		cout << "cacheReplyPckage size is: " << cacheReplyPckage.size() << endl;
			switch (reply->code) {
			case ServerToClientPackageHdr::GAME_TABLE_INFO: {
				endTurn();
				cout << "GAME_TABLE_INFO" << endl;
				printTableInfo (static_pointer_cast<GameTableInfo>(reply));
				break;			}
			case ServerToClientPackageHdr::TOUR_START:
			{
				cout << "TOUR_START" << endl;
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
				cout << "The winner is " << static_cast<EndGameResult*>(reply.get())->m_winner << endl;
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
//	else {
//		cout << "Error: Reply is empty in handleGamePackageData " << endl;
//	}
		usleep(100000);
}while(!reply || reply->code != ServerToClientPackageHdr::END_GAME);
}

bool PokerClient::isTurn() const {
	return turn;
}

void PokerClient::endTurn() {
	turn = false;
}

void PokerClient::printTableInfo(shared_ptr<GameTableInfo> gameTableInfo) {
	cout << "Card on table: " << endl;
	for (int i = 0; i < CARDS_ON_TABLE; i++) {
		printCard(gameTableInfo->m_cardOnTable[i]);
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
		printCard(pinf.pocketCards_.firstCard_);
		cout << "secondCard_: ";
		printCard(pinf.pocketCards_.secondCard_);
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

void PokerClient::printCard(const GameCard& card) const {
	if (!card.getRank()) {
		cout << "None" << endl;
		return;
	}
	string str;
	switch (card.getSuit()) {
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
	switch (card.getRank()) {
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
