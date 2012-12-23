/*
 * CConnect.hpp
 *
 *  Created on: 22.12.2012
 *      Author: Alexey
 */

#ifndef CCONNECT_HPP_
#define CCONNECT_HPP_

#include "Command.hpp"

class CConnect: public Command
{
public:
	CConnect( PokerClient *pokerClient ) :
	Command( pokerClient, "connect", "Connect to the server.\n", "\nhost\n", "Default is localhost \n" )
	{
	}
	virtual ~CConnect()
	{
	}
	virtual void Execute( const vector<string> &args )
		{
			if ( args.size() == 1 || args.size() == 2 ) {
				if ( args.size() == 1 ) {
					cout << "Connecting to localhost:3567\n" << endl;
					m_pokerClient->connect( "localhost" , "3567" );
				} else {
					cout << "Connecting to server: " << args[1] << endl;
					m_pokerClient->connect( args[1], "3567" );
				}
				return;
			}
			cerr << GetUsage() << "\n";
		}
};

#endif /* CCONNECT_HPP_ */
