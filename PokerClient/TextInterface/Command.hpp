/*
 * Command.h
 *
 *  Created on: 22.12.2012
 *      Author: Alexey
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <string>
#include <vector>
#include <ostream>
#include "../PokerClient.h"

using namespace std;

class Command
{
public:
	Command(PokerClient *pokerClient, const string &stName, const string &stHelp, const string &stUsage, const string &stDetails ) :
		m_pokerClient(pokerClient), m_Name( stName ), m_stShortHelp( stHelp ), m_stDetails( stDetails )
	{
		stringstream usage( stUsage );
		string line;
		m_stUsage = "Usage:\n";
		while ( getline( usage, line ) ) {
			m_stUsage += "\t" + m_Name + " " + line + "\n";
		}
	}
	virtual ~Command()
	{
	}

	virtual void Execute( const vector<string> &args ) = 0;

	virtual void GetCompleteList( const string &begin, list<string> &CompleteList )
	{
		CompleteList.clear();
	}

	const string &GetName()
	{
		return m_Name;
	}
	virtual const string GetShortHelp()
	{
		return m_stShortHelp;
	}
	virtual const string GetUsage()
	{
		return m_stUsage;
	}
	virtual const string GetLongHelp()
	{
		return m_stShortHelp + "\n" + m_stUsage + "\n" + m_stDetails;
	}
protected:
	static const size_t MAX_COLS = 80;
	PokerClient* m_pokerClient;
	const string m_Name;
	string m_stShortHelp;
	string m_stDetails;
	string m_stUsage;
};

#endif /* COMMAND_H_ */
