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

#ifndef COMMANDSLIST_H_
#define COMMANDSLIST_H_

#include <vector>

using namespace std;

class PokerClient;
class Command;

class CommandsList
{
public:
	CommandsList( PokerClient *pokerClient );
	virtual ~CommandsList();

	const vector<Command*> &GetList()
	{
		return m_Commands;
	}
private:
	vector<Command*> m_Commands;
};

#endif /* COMMANDSLIST_H_ */
