/*
 * CConnect.hpp
 *
 *  Created on: 22.12.2012
 *      Author: Alexey
 */

#ifndef CPRINT_ALL_TOUR_ID_HPP_
#define CPRINT_ALL_TOUR_ID_HPP_

#include "Command.hpp"
#include <cstdlib>

class CPrintAllTourID: public Command
{
public:
	CPrintAllTourID( PokerClient *pokerClient ) :
	Command( pokerClient, "printalltourid", "Print all available tourID.\n", "\n", "\n" )
	{
	}
	virtual ~CPrintAllTourID()
	{
	}
	virtual void Execute( const vector<string> &args )
	{
		if ( args.size() == 1 ) {
			m_pokerClient->printAllTourID();
			return;
		}

		cerr << GetUsage() << "\n";
	}

};

#endif /* CPRINT_ALL_TOUR_ID_HPP_ */
