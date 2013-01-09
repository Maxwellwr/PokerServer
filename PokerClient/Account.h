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

#ifndef ACCOUNT_H_
#define ACCOUNT_H_

#include <stdint.h>
#include "PlayerInfo.hpp"

enum AccountType {
	BOT,
	HUMAN,
	LOGGER
};

class Account
{
public:
	Account();

	virtual ~Account();

	virtual uint8_t getType() const;
	virtual const char* getName() const;
protected:
	Account(uint8_t clientType);

	uint8_t clientType_;
};

class Player: public Account
{
public:
	Player(const char *name);
	virtual ~Player();

	void setName(const char *name);
	virtual const char* getName() const;


private:
	PlayerInfo info;
};

#endif /* ACCOUNT_H_ */
