/*
 * CConnect.hpp
 *
 *  Created on: 22.12.2012
 *      Author: Alexey
 */

#ifndef CCREATE_TOUR_HPP_
#define CCREATE_TOUR_HPP_

#include "Command.hpp"
#include <cstdlib>

class CCreateTour: public Command
{
public:
	CCreateTour( PokerClient *pokerClient ) :
	Command( pokerClient, "createtour", "Create a tour on the server.\n", "\nwait_time maxPlayers\n", "\n" )
	{
	}
	virtual ~CCreateTour()
	{
	}
	virtual void Execute( const vector<string> &args )
	{
		if ( args.size() == 3 ) {
			auto tourID = m_pokerClient->createTour(atoi(args[1].c_str()),atoi(args[2].c_str()));
			cout << "Tour[" << tourID << "] has been created." << endl;
			return;
		}

		cerr << GetUsage() << "\n";
	}

};

#endif /* CCREATE_TOUR_HPP_ */
