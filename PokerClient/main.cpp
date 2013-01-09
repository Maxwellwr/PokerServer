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

