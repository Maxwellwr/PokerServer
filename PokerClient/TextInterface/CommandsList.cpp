/*
 * CommandsList.cpp
 *
 *  Created on: 22.12.2012
 *      Author: Alexey
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

#include "CExit.hpp"
#include "CConnect.hpp"
#include "CCreateTour.hpp"
#include "CSelectTour.hpp"
#include "CGetTourInfo.hpp"
#include "CGetPlayersInfo.hpp"
#include "CPrintTourInfo.hpp"
#include "CPrintAllTourID.hpp"


CommandsList::CommandsList( PokerClient *pokerClient )
{
	m_Commands.push_back( new CExit( pokerClient, "exit") );
	m_Commands.push_back( new CExit( pokerClient, "quit") );
	m_Commands.push_back( new CConnect( pokerClient) );
	m_Commands.push_back( new CCreateTour( pokerClient) );
	m_Commands.push_back( new CSelectTour( pokerClient ) );
	m_Commands.push_back( new CGetTourInfo( pokerClient ) );
	m_Commands.push_back( new CGetPlayersInfo( pokerClient ) );
	m_Commands.push_back( new CPrintTourInfo( pokerClient ) );
	m_Commands.push_back( new CPrintAllTourID( pokerClient ) );




}

CommandsList::~CommandsList()
{
	for ( size_t i = 0; i < m_Commands.size(); i++ ) {
		delete m_Commands[i];
	}
}

