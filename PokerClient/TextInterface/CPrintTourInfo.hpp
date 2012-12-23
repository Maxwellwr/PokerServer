/*
 * CConnect.hpp
 *
 *  Created on: 22.12.2012
 *      Author: Alexey
 */

#ifndef CPRINT_TOUR_INFO_HPP_
#define CPRINT_TOUR_INFO_HPP_

#include "Command.hpp"
#include <cstdlib>

class CPrintTourInfo: public Command
{
public:
	CPrintTourInfo( PokerClient *pokerClient ) :
	Command( pokerClient, "printtourinfo", "Print a tour info.\n", "\ntourID\n", "\n" )
	{
	}
	virtual ~CPrintTourInfo()
	{
	}
	virtual void Execute( const vector<string> &args )
	{
		if ( args.size() == 2 ) {
			auto tourID = atoi(args[1].c_str());
			m_pokerClient->getTourInfo(tourID);
			m_pokerClient->getPlayersInfo(tourID);
			m_pokerClient->printTourInfo(tourID);
			return;
		}

		cerr << GetUsage() << "\n";
	}

};

#endif /* CPRINT_TOUR_INFO_HPP_ */
