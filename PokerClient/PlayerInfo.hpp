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

#ifndef PLAYERINFO_H_
#define PLAYERINFO_H_

#include <stdint.h>
#include <cstring>

#pragma pack(push, 1)

class GameCard
{
public:
	enum CardSuit : uint8_t
	{
		Clubs = 0,
		Diamond = 1,
		Hearts = 2,
		Spades = 3
	};

	enum CardRank : uint8_t
	{
		none = 0,
		Ace = 1,
		Two = 2,
		Three = 3,
		Four = 4,
		Five = 5,
		Six = 6,
		Seven = 7,
		Eight = 8,
		Nine = 9,
		Ten = 10,
		Jack = 11,
		Queen = 12,
		King = 13
	};

	uint8_t getSuit() const {return suit_ & 0x3;}
	uint8_t getRank() const {return rank_& 0xf;}

//private:
	uint8_t rank_ :4;
	uint8_t suit_ :2;

};

struct HandInfo
{
	GameCard firstCard_;
	GameCard secondCard_;
};

struct PlayerInfo
{
	char name_[15];
	uint32_t stack_;
	uint8_t lastAction_;
	uint32_t currentBet_;
	HandInfo pocketCards_;

	PlayerInfo() :
			name_(), stack_( -1 ), lastAction_( -1 ), currentBet_(-1)
	{
	}
	PlayerInfo( const char *name ) :
			PlayerInfo()
	{
		setName( name );
	}

	void setName( const char *name )
	{
		uint8_t nameLength = strlen( name );
		memcpy( name_, name, nameLength );
	}
};

#pragma pack(pop)

#endif /* PLAYERINFO_H_ */
