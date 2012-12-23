/*
 * PlayerInfo.h
 *
 *  Created on: 23.12.2012
 *      Author: Alexey
 */

#ifndef PLAYERINFO_H_
#define PLAYERINFO_H_

#include <stdint.h>
#include <cstring>

struct HandInfo
{
	uint8_t firstCard_;
	uint8_t secondCard_;

	HandInfo() : firstCard_(-1), secondCard_(-1)
	{
	}
};

struct PlayerInfo
{
	char name_[15];
	uint32_t stack_;
	uint8_t lastAction_;
	bool isActive_;
	HandInfo pocketCards_;

	PlayerInfo():name_(), stack_(-1), lastAction_(-1), isActive_(false)
	{
	}
	PlayerInfo(const char *name) : PlayerInfo()
	{
		setName(name);
	}

	void setName(const char *name)
	{
		uint8_t nameLength = strlen(name);
		memcpy( name_, name, nameLength );
	}
};

#endif /* PLAYERINFO_H_ */
