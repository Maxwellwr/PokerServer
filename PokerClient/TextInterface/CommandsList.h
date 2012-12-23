/*
 * CommandsList.h
 *
 *  Created on: 22.12.2012
 *      Author: Alexey
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
