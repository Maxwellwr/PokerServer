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

#ifndef TEXTINTERFACE_H_
#define TEXTINTERFACE_H_

#include <string>
#include <vector>

#include "CommandsList.h"

using namespace std;

class Command;

class TextInterface
{
public:
	TextInterface(PokerClient *pokerClient);
	virtual ~TextInterface();

	void ExecuteCommand( const char *cCommand );

	Command *GetCompleteList( const string &cCommandBegining, list<string> &CompleteList );

	bool IsThisRepeatCommand();
private:

	void SplitArgs( const string &Command, vector<string> &Args );

	bool IsSpaceStr( const char* s );

	Command *m_PrevCommand;
	vector<string> m_PrevArgs;
	unsigned long m_ulRepeatNumber;
	CommandsList m_CommandsList;

};

#endif /* TEXTINTERFACE_H_ */
