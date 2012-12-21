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
	uint32_t createTime;
	uint32_t waitTime;
	uint32_t beginTime;
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

private:
	shared_ptr<ConnectSession> connectSession_;
	shared_ptr<Account> account_;
	map<uint32_t, shared_ptr<Tour>> tours_;
//	shared_ptr<ServerToClientPackageHdr> serverReply_;
};

#endif /* POKERCLIENT_H_ */
