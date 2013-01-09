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

#ifndef POKER_GAME_COMMANDS_HPP_
#define POKER_GAME_COMMANDS_HPP_

#include "Command.hpp"

class Call: public Command
{
public:
	Call( PokerClient *pokerClient ) :
	Command( pokerClient, "call", "Call.\n", "\n", "\n" )
	{
	}
	virtual ~Call()
	{
	}
	virtual void Execute( const vector<string> &args )
	{
		if ( args.size() == 1 ) {
			m_pokerClient->call();
			return;
		}
		cerr << GetUsage() << "\n";
	}

};

class Rise: public Command
{
public:
	Rise( PokerClient *pokerClient ) :
	Command( pokerClient, "rise", "Rise.\n", "\nmoney\n", "\n" )
	{
	}
	virtual ~Rise()
	{
	}
	virtual void Execute( const vector<string> &args )
	{
		if ( args.size() == 2 ) {
			m_pokerClient->rise(atoi(args[1].c_str()));
			return;
		}
		cerr << GetUsage() << "\n";
	}

};

class Fald: public Command
{
public:
	Fald( PokerClient *pokerClient ) :
	Command( pokerClient, "fald", "Fald.\n", "\n", "\n" )
	{
	}
	virtual ~Fald()
	{
	}
	virtual void Execute( const vector<string> &args )
	{
		if ( args.size() == 1 ) {
			m_pokerClient->fald();
			return;
		}
		cerr << GetUsage() << "\n";
	}

};

#endif /* POKER_GAME_COMMANDS_HPP_ */
