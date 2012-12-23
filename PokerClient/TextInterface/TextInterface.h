/*
 * TextInterface.h
 *
 *  Created on: 22.12.2012
 *      Author: Alexey
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
