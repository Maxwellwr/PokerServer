/*
 * Account.cpp
 *
 *  Created on: 16.12.2012
 *      Author: Alexey
 */

#include "Account.h"
#include <cstring>

Account::Account() :
		clientType_( -1 )
{
	// TODO Auto-generated constructor stub

}

Account::Account(uint8_t clientType):clientType_(clientType)
{
}

Account::~Account()
{
	// TODO Auto-generated destructor stub
}

uint8_t Account::getType() const
{
	return clientType_;
}

const char* Account::getName() const
{
	return 0;
}

Player::Player( const char *name ) :
		Account( HUMAN ), info(name)
{
}

Player::~Player()
{
	// TODO Auto-generated destructor stub
}

const char* Player::getName() const
{
	return info.name_;
}

void Player::setName( const char *name)
{
	info.setName(name);
}
