/*
 * CPokerCommand.hpp
 *
 *  Created on: 24.12.2012
 *      Author: Alexey
 */

#ifndef CPOKERCOMMAND_HPP_
#define CPOKERCOMMAND_HPP_

class Call: public Command
{
public:
	Call( PokerClient *pokerClient ) :
	Command( pokerClient, "call", "Call.\n", "\n", "\n" )
	{
	}
	virtual ~Call()
	{
	}
	virtual void Execute( const vector<string> &args )
	{
		if ( args.size() == 1 ) {
			m_pokerClient->call();
			return;
		}
		cerr << GetUsage() << "\n";
	}

};

class Rise: public Command
{
public:
	Rise( PokerClient *pokerClient ) :
	Command( pokerClient, "rise", "Rise.\n", "\nmoney\n", "\n" )
	{
	}
	virtual ~Rise()
	{
	}
	virtual void Execute( const vector<string> &args )
	{
		if ( args.size() == 2 ) {
			m_pokerClient->rise(atoi(args[1].c_str()));
			return;
		}
		cerr << GetUsage() << "\n";
	}

};

class Fald: public Command
{
public:
	Fald( PokerClient *pokerClient ) :
	Command( pokerClient, "fald", "Fald.\n", "\n", "\n" )
	{
	}
	virtual ~Fald()
	{
	}
	virtual void Execute( const vector<string> &args )
	{
		if ( args.size() == 1 ) {
			m_pokerClient->fald();
			return;
		}
		cerr << GetUsage() << "\n";
	}

};

#endif /* CPOKERCOMMAND_HPP_ */
