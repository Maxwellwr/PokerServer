/* Copyright (c) 2012, 2013 Q-stat.
 *
 *	This file is part of PokerClient.
 *
 *	PokerClient is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 * PokerClient is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with PokerServer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BASE_COMMANDS_HPP_
#define BASE_COMMANDS_HPP_

#include <cstdlib>
#include "Command.hpp"

class Connect: public Command
{
public:
	Connect( PokerClient *pokerClient ) :
	Command( pokerClient, "connect", "Connect to the server.\n", "\nhost\n", "Default is localhost \n" )
	{
	}
	virtual ~Connect()
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

class Exit: public Command
{
public:
	Exit( PokerClient *pokerClient, const char *name ) :
	Command( pokerClient, name, "Exit current session.\n", "\n", "\n" )
	{
	}
	virtual ~Exit()
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

class CreateTour: public Command
{
public:
	CreateTour( PokerClient *pokerClient ) :
	Command( pokerClient, "createTour", "Create a tour on the server.\n", "\nwait_time maxPlayers\n", "\n" )
	{
	}
	virtual ~CreateTour()
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

class SelectTour: public Command
{
public:
	SelectTour( PokerClient *pokerClient ) :
	Command( pokerClient, "select", "Select a tour on the server.\n", "\ntourID\n", "\n" )
	{
	}
	virtual ~SelectTour()
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

class PrintAllToursIDs: public Command
{
public:
	PrintAllToursIDs( PokerClient *pokerClient ) :
	Command( pokerClient, "printAllToursIds", "Print all available toursIDs.\n", "\n", "\n" )
	{
	}
	virtual ~PrintAllToursIDs()
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

class PrintTourInfo: public Command
{
public:
	PrintTourInfo( PokerClient *pokerClient ) :
	Command( pokerClient, "printtourinfo", "Print a tour info.\n", "\ntourID\n", "\n" )
	{
	}
	virtual ~PrintTourInfo()
	{
	}
	virtual void Execute( const vector<string> &args )
	{
		if ( args.size() == 2 ) {
			auto tourID = atoi(args[1].c_str());
			m_pokerClient->printTourInfo(tourID);
			return;
		}

		cerr << GetUsage() << "\n";
	}

};

#endif /* BASE_COMMANDS_HPP_ */
