/*
 * Account.h
 *
 *  Created on: 16.12.2012
 *      Author: Alexey
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
