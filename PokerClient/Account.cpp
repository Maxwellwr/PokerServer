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
