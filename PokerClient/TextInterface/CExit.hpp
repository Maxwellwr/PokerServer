/*
 * CExit.h
 *
 *  Created on: 22.12.2012
 *      Author: Alexey
 */

#ifndef CEXIT_HPP_
#define CEXIT_HPP_

#include "Command.hpp"

class CExit: public Command
{
public:
	CExit( PokerClient *pokerClient, const char *name ) :
	Command( pokerClient, name, "Exit current session.\n", "\n", "\n" )
	{
	}
	virtual ~CExit()
	{
	}
	virtual void Execute( const vector<string> &args )
		{
			if ( args.size() != 1 ) {
				cerr << GetUsage() << "\n";
			}
			exit( 0 );
		}
};

#endif /* CEXIT_HPP_ */
