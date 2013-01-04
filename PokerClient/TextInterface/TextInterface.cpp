/*
 * TextInterface.cpp
 *
 *  Created on: 22.12.2012
 *      Author: Alexey
 */

#include <limits.h>
#include <errno.h>
#include <stdlib.h>

#include "Command.hpp"
#include "TextInterface.h"

TextInterface::TextInterface(PokerClient *pokerClient) : m_CommandsList(pokerClient)
{
	m_PrevCommand = NULL;
	m_ulRepeatNumber = 0;
}

TextInterface::~TextInterface()
{

}

void TextInterface::SplitArgs( const string &Command, vector<string> &Args )
{
	size_t QuotePtr, SplitPtr, CurrentPtr = 0;
	size_t len, end; //, i;

	Args.clear();
	end = 0;

	len = Command.length();
	do {
		QuotePtr = Command.find_first_of( '"', CurrentPtr );
		SplitPtr = Command.find_first_of( ' ', CurrentPtr );
		if ( Command.find_first_of( ',', CurrentPtr ) != string::npos
				&& (Command.find_first_of( ',', CurrentPtr ) < SplitPtr || SplitPtr == string::npos) ) {
			SplitPtr = Command.find_first_of( ',', CurrentPtr );
		}

		if ( QuotePtr != string::npos ) {
			if ( SplitPtr != string::npos ) {
				if ( QuotePtr < SplitPtr ) {
					if ( Command.at( CurrentPtr ) != '"' ) {
						Args.clear();
						return;
					}
					CurrentPtr = Command.find_first_of( '"', QuotePtr + 1 );
					if ( CurrentPtr == string::npos ) {
						Args.clear();
						return;
					}
					Args.push_back( Command.substr( QuotePtr + 1, CurrentPtr - QuotePtr - 1 ) );
					CurrentPtr++;
					if ( CurrentPtr < len ) {
						char c = Command.at( CurrentPtr );
						if ( c == ' ' || c == ',' || c == '\t' || c == '=' ) {
							CurrentPtr++;
						}
					}
				} else {
					Args.push_back( Command.substr( CurrentPtr, SplitPtr - CurrentPtr ) );
					CurrentPtr = SplitPtr + 1; // TODO: про€снить что за команда
				}
			} else {
				if ( Command.at( CurrentPtr ) != '"' ) {
					Args.clear();
					return;
				}
				CurrentPtr = Command.find_first_of( '"', QuotePtr + 1 );
				if ( CurrentPtr == string::npos ) {
					Args.clear();
					return;
				}
				Args.push_back( Command.substr( QuotePtr + 1, CurrentPtr - QuotePtr - 1 ) );
				CurrentPtr++;
				if ( CurrentPtr < Command.length() ) {/* must be the last thing */
					Args.clear();
					return;
				}
				end = 1;
			}
		} else if ( SplitPtr != string::npos ) {
			Args.push_back( Command.substr( CurrentPtr, SplitPtr - CurrentPtr ) );
			CurrentPtr = SplitPtr + 1;

		} else {
			if ( CurrentPtr < Command.length() ) {
				Args.push_back( Command.substr( CurrentPtr, string::npos/*QuotePtr+1, len-QuotePtr-2*/) );
			}
			end = 1;
		}
	} while ( !end );

}


bool TextInterface::IsSpaceStr( const char* s )
{
	if ( !s ) return true;
	while ( *s ) {
		if ( (*s == ' ') || (*s == '\t') || (*s == '\r') || (*s == '\n') ) s++;
		else return false;
	};
	return true;
}

void TextInterface::ExecuteCommand( const char *cCommand )
{
	string str;
	vector<string> args;

	// our routine which allows for "" delimited parameters containing (almost) arbitrary characters

	str = cCommand;
	SplitArgs( str, args );
	for ( vector<string>::iterator it = args.begin(); it < args.end(); ) {
		// »гнорируем пустые строки
		if ( IsSpaceStr( (*it).c_str() ) ) {
			it = args.erase( it );
		} else {
			size_t TruncPtr;
			TruncPtr = min( (*it).find_last_of( '\n' ), (*it).find_last_of( '\r' ) );
			if ( TruncPtr != string::npos ) {
				(*it).erase( TruncPtr, (*it).length() - TruncPtr );
			}
			++it;
		}
	}

	/* Execute a command */
	if ( args.size() == 0 ) {
		/* Empty line */
		if ( m_PrevCommand && cCommand[0] == '\0' ) {
			/* Repeat previous command */
			m_ulRepeatNumber++;
			m_PrevCommand->Execute( m_PrevArgs );
			return;
		} else {
			return;
		}
	}
	/*
	 * Guess command name.
	 */
	list<string> GuessedCommands;
	Command *cmd = GetCompleteList( args[0], GuessedCommands );

	if ( GuessedCommands.size() ) {
		args[0] = GuessedCommands.front();
	} else {
		cerr << "Unknown command\n";
		m_PrevArgs.clear();
		return;
	}

	m_ulRepeatNumber = 0;
	if ( cmd ) {
		cmd->Execute( args );
		/*
		 * Save command parameters only for
		 * interactive mode.
		 */
		m_PrevCommand = cmd;
		m_PrevArgs = args;
	}

}

Command *TextInterface::GetCompleteList( const string &cCommandBegining, list<string> &CompleteList )
{
	CompleteList.clear();
	const vector<Command*> list = m_CommandsList.GetList();
	Command *result = NULL;

		for ( size_t i = 0; i < list.size(); i++ ) {
			const string &cmd = list[i]->GetName();
			if ( cmd.find( cCommandBegining ) == 0 ) {
				CompleteList.push_back( cmd );
				if ( !result ) {
					result = list[i];
				}
			}
		}
//	}
	return result;
}

bool TextInterface::IsThisRepeatCommand()
{
	return m_ulRepeatNumber > 0;
}
