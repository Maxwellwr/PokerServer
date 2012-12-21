/*
 * main.cpp
 *
 *  Created on: 01.12.2012
 *      Author: Alexey
 */

#include <iostream>
#include <thread>
#include "asio.hpp"
#include "PackageStructure.hpp"
#include "PokerClient.h"

using namespace std;
using asio::ip::tcp;

int main( int argc, char** argv )
{
	try {
		char nick[] = "userName";
		shared_ptr<Account> account( new Player( nick, sizeof(nick) ) );
		PokerClient client(account);
		client.connect("95.24.12.137", "3567");	//95.24.12.137
//		client.getTourInfo(10);
		auto tourID = client.createTour( 300 , 4);
		client.getPlayersInfo(tourID);
	} catch (exception& e) {
		cout << e.what() << endl;
	}

	return 0;
}

