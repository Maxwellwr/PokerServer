/*
 * CConnect.hpp
 *
 *  Created on: 22.12.2012
 *      Author: Alexey
 */

#ifndef CGET_PLAYERS_INFO_HPP_
#define CGET_PLAYERS_INFO_HPP_

#include "Command.hpp"
#include <cstdlib>

class CGetPlayersInfo: public Command
{
public:
	CGetPlayersInfo( PokerClient *pokerClient ) :
	Command( pokerClient, "getplayersinfo", "Get a tour's players info.\n", "\ntourID\n", "\n" )
	{
	}
	virtual ~CGetPlayersInfo()
	{
	}
	virtual void Execute( const vector<string> &args )
	{
		if ( args.size() == 2 ) {
			m_pokerClient->getPlayersInfo(atoi(args[1].c_str()));
			cout << "Tour[" << args[1] << "]'s players info has been acquired." << endl;
			return;
		}

		cerr << GetUsage() << "\n";
	}

};

#endif /* CGET_PLAYERS_INFO_HPP_ */
