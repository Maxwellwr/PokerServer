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
