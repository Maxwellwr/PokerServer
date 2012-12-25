/*
 * PokerClient.h
 *
 *  Created on: 16.12.2012
 *      Author: Alexey
 */

#ifndef POKERCLIENT_H_
#define POKERCLIENT_H_

#include <memory>
#include <iostream>
#include <set>
#include "asio.hpp"
#include "Account.h"
#include "PackageStructure.hpp"
#include "ConnectSession.h"

using asio::ip::tcp;
using namespace std;

struct Tour
{
	uint32_t tourID;
	uint32_t maxPlayers;
	uint32_t freePlaces;
	int64_t createTime;
	uint32_t waitTime;
	int64_t beginTime;
	set<string> players;

	Tour() :
		tourID( -1 ), maxPlayers( 0 ), freePlaces( 0 ), createTime( 0 ), waitTime( 0 ), beginTime( 0 )
	{
	}
};

class PokerClient
{
public:
	PokerClient( shared_ptr<Account> account);
	~PokerClient();

	void connect(const string address, const string port);

	void getTourInfo(uint32_t tourID);
	void selectTour(uint32_t tourID);
	uint32_t createTour(uint32_t waitTime, uint32_t maxPlayers);
	void getPlayersInfo(uint32_t tourID);
	void printTourInfo(uint32_t tourID);
	void printAllTourID();

	void getTours();

	void call();
	void rise(uint32_t money);
	void fald();


private:
	void handleGamePackageData(shared_ptr<ServerToClientPackageHdr> reply);
	bool isTurn() const;
	void endTurn();
	void printTableInfo(shared_ptr<GameTableInfo> gameTableInfo);
	void printCard(const GameCard& card) const;

	shared_ptr<ConnectSession> connectSession_;
	shared_ptr<Account> account_;
	map<uint32_t, shared_ptr<Tour>> tours_;
	vector<shared_ptr<ServerToClientPackageHdr>> cacheReplyPckage;
	bool turn;

};

#endif /* POKERCLIENT_H_ */
