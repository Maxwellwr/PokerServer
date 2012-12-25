/*
 * main.cpp
 *
 *  Created on: 01.12.2012
 *      Author: Alexey
 */

#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>
#include "PackageStructure.hpp"
#include "PokerClient.h"

#include "TextInterface/TextInterface.h"

TextInterface *ti;

char *generator( const char *text, int state )
{
	static list<string> stCompleters;

	if (!state) {
		ti->GetCompleteList( string( rl_line_buffer ), stCompleters );
	}
	if (state < (int) stCompleters.size()) {
		list<string>::iterator it = stCompleters.begin();
		for (int i = 0; i < state; i++) {
			it++;
		}
		return strdup( (*it).c_str() );
	} else {
		stCompleters.clear();
		return NULL;
	}
}

char ** completionHandler( const char *text, int start, int end )
{
	char **matches = NULL;
	matches = rl_completion_matches( text, generator );
	return matches;
}

void readline_init( void )
{
#ifdef DEBUG
	_printf("Readline version: %s\n", rl_library_version);
#endif
	rl_initialize();
	rl_readline_name = "PokerClient";
	rl_attempted_completion_function = completionHandler;
}

int main( int argc, char** argv )
{
//	char nick[] = "userName";
	shared_ptr<Account> account( new Player( argv[1] ) );

	char *line = NULL;
	const char *prompt = "pokerClient> ";
	try {
		PokerClient client( account );
		TextInterface textinterface( &client );
		ti = &textinterface;

		readline_init();

		while (true) {
			if ((line = readline( prompt )) == NULL) {
				putc( '\n', stdout);
				exit( 0 );
			}
			try {
				ti->ExecuteCommand( line );
				add_history( line );
			} catch (exception &e) {
				cerr << "Error: " << e.what() << endl;
			}

			if (line) {
				free( line );
				line = NULL;
			}
		}
	} catch (...) {
		cerr << "Unknown exception." << endl;
		return 1;
	}

	return 0;
}

