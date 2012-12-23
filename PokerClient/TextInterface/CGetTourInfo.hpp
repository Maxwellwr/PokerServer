/*
 * CConnect.hpp
 *
 *  Created on: 22.12.2012
 *      Author: Alexey
 */

#ifndef CGET_TOUR_INFO_HPP_
#define CGET_TOUR_INFO_HPP_

#include "Command.hpp"
#include <cstdlib>

class CGetTourInfo: public Command
{
public:
	CGetTourInfo( PokerClient *pokerClient ) :
	Command( pokerClient, "get_tour_info", "Get a tour info.\n", "\ntourID\n", "\n" )
	{
	}
	virtual ~CGetTourInfo()
	{
	}
	virtual void Execute( const vector<string> &args )
	{
		if ( args.size() == 2 ) {
			m_pokerClient->getTourInfo(atoi(args[1].c_str()));
			cout << "Tour[" << args[1] << "]'s info has been acquired." << endl;
			return;
		}

		cerr << GetUsage() << "\n";
	}

};

#endif /* CGET_TOUR_INFO_HPP_ */
