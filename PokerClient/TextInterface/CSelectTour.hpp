/*
 * CConnect.hpp
 *
 *  Created on: 22.12.2012
 *      Author: Alexey
 */

#ifndef CSELECT_TOUR_HPP_
#define CSELECT_TOUR_HPP_

#include "Command.hpp"
#include <cstdlib>

class CSelectTour: public Command
{
public:
	CSelectTour( PokerClient *pokerClient ) :
	Command( pokerClient, "select", "Select a tour on the server.\n", "\ntourID\n", "\n" )
	{
	}
	virtual ~CSelectTour()
	{
	}
	virtual void Execute( const vector<string> &args )
	{
		if ( args.size() == 2 ) {
			m_pokerClient->selectTour(atoi(args[1].c_str()));
			cout << "Tour[" << args[1] << "] has been selected." << endl;
			return;
		}

		cerr << GetUsage() << "\n";
	}

};

#endif /* CSELECT_TOUR_HPP_ */
