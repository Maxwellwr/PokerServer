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

#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string.h>

using namespace std;

#include "../PokerClient.h"
#include "CommandsList.h"

#include "PokerGameCommands.hpp"
#include "BaseCommands.hpp"


CommandsList::CommandsList( PokerClient *pokerClient )
{
	m_Commands.push_back( new Exit( pokerClient, "exit") );
	m_Commands.push_back( new Exit( pokerClient, "quit") );
	m_Commands.push_back( new Connect( pokerClient) );
	m_Commands.push_back( new CreateTour( pokerClient) );
	m_Commands.push_back( new SelectTour( pokerClient ) );
	m_Commands.push_back( new PrintTourInfo( pokerClient ) );
	m_Commands.push_back( new PrintAllToursIDs( pokerClient ) );
	m_Commands.push_back( new Call( pokerClient ) );
	m_Commands.push_back( new Rise( pokerClient ) );
	m_Commands.push_back( new Fald( pokerClient ) );

}

CommandsList::~CommandsList()
{
	for ( size_t i = 0; i < m_Commands.size(); i++ ) {
		delete m_Commands[i];
	}
}

