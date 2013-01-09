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

	void selectTour(uint32_t tourID);
	uint32_t createTour(uint32_t waitTime, uint32_t maxPlayers);
	void printTourInfo(uint32_t tourID);
	void printAllTourID();

	void getTours();

	void call();
	void rise(uint32_t money);
	void fald();


private:
	void handleGamePackageData(/*shared_ptr<ServerToClientPackageHdr> reply*/);
	bool isTurn() const;
	void endTurn();
	void getTourInfo(uint32_t tourID);
	void getPlayersInfo(uint32_t tourID);
	void printTableInfo(shared_ptr<GameTableInfoPkg> gameTableInfo);
	void printCard(const GameCard& card) const;

	shared_ptr<std::thread> handleThread;
	shared_ptr<ConnectSession> connectSession_;
	shared_ptr<Account> account_;
	map<uint32_t, shared_ptr<Tour>> tours_;
	bool turn;
//	bool isValid_;

};

#endif /* POKERCLIENT_H_ */
