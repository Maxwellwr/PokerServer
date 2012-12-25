/* Copyright (c) Q-stat 2012.
 *
 *	This file is part of PokerServer.
 *
 *	PokerServer is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 * PokerServer is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with PokerServer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PACKAGESTRUCTURE_HPP_
#define PACKAGESTRUCTURE_HPP_

#include <algorithm>
#include <string.h>
#include <stdint.h>
#include <memory>
#include "PlayerInfo.hpp"
#include "BankInfo.hpp"

#pragma pack(push, 1)

struct PackageHdr
{
	uint32_t code;
	uint32_t bodyLength;
};

template<typename PackageType>
	std::shared_ptr<PackageType> appendBody( const PackageType& packageHdr,
			char* inBuf, uint32_t inBufLength )
	{
		const PackageHdr &packet_hdr = packageHdr;
		uint32_t new_body_length = packet_hdr.bodyLength + inBufLength;
		char *buf = new char[sizeof(PackageHdr) + new_body_length];
		memcpy( buf, &packet_hdr, sizeof(PackageHdr) + packet_hdr.bodyLength );
		memcpy( buf + sizeof(PackageHdr) + packet_hdr.bodyLength, inBuf,
				inBufLength );
		PackageType *hdr = (PackageType *) buf;
		hdr->bodyLength = new_body_length;
		return std::shared_ptr < PackageType
				> (hdr, std::default_delete<PackageType[]>());
	}

struct ServerToClientPackageHdr : public PackageHdr
{
	enum ServCommandCode
	{
		CONNECT_OK = 256,
		TOUR_INFO = 257,
		SELECT_OK = 258,
		PLAYERS_INFO = 259,
		ERROR_INFO = 260,
		TOUR_START = 261,
		GAME_TABLE_INFO = 262,
		GAME_TABLE_ACTION_INVITE = 263,
		END_GAME_RESULT = 264,
		END_GAME = 265
	};
};

struct ConnectOk : public ServerToClientPackageHdr
{
	uint32_t m_toursCount;
	uint32_t m_toursIDs[0];
	ConnectOk( uint32_t toursCount ) :
			m_toursCount( toursCount )
	{
		code = CONNECT_OK;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

struct TourInfo : public ServerToClientPackageHdr
{
	uint32_t m_tourID;
	uint32_t m_freePlaces;
	int64_t m_creationTime;
	uint32_t m_waitTime;

	TourInfo( uint32_t tourID, uint32_t freePlaces, uint32_t creationTime,
			uint32_t waitTime ) :
			m_tourID( tourID ), m_freePlaces( freePlaces ),
			m_creationTime( creationTime ), m_waitTime( waitTime )
	{
		code = TOUR_INFO;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

struct SelectOk : public ServerToClientPackageHdr
{
	uint32_t m_tourID;
	int64_t m_beginTime;

	SelectOk( uint32_t tourID, uint32_t beginTime ) :
			m_tourID( tourID ), m_beginTime( beginTime )
	{
		code = SELECT_OK;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

struct PlayersInfo : public ServerToClientPackageHdr
{
	uint32_t m_tourID;
	uint32_t m_playersCount;
	uint8_t m_playersNames[0];

	PlayersInfo( uint32_t tourID, uint32_t playersCount ) :
			m_tourID( tourID ), m_playersCount( playersCount )
	{
		code = PLAYERS_INFO;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

struct ErrorFromServer : public ServerToClientPackageHdr
{
	uint8_t m_errorMessage[0];

	ErrorFromServer()
	{
		code = ERROR_INFO;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

struct TourStart : public ServerToClientPackageHdr
{
	uint32_t m_tourID;

	TourStart( uint32_t tourID ) :
			m_tourID( tourID )
	{
		code = TOUR_START;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

#define CARDS_ON_TABLE 5

struct GameTableInfo : public ServerToClientPackageHdr
{
	GameCard m_cardOnTable[CARDS_ON_TABLE];
	uint16_t m_playersCount;
	PlayerInfo *m_playersInfo;
	BankInfo *m_bank;

	GameTableInfo( uint32_t playersCount ) :
			m_playersCount( playersCount ),
			m_playersInfo(0), m_bank(0)
	{
		code = GAME_TABLE_INFO;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}

	PlayerInfo* getPlayerInfoPntr() {return (PlayerInfo*)((char*)&m_playersInfo );}
	BankInfo* getBankInfoPntr() {return (BankInfo*)((char*)&m_playersInfo + sizeof(PlayerInfo) * m_playersCount);}

};

struct ActionInvite : public ServerToClientPackageHdr
{
	ActionInvite()
	{
		code = GAME_TABLE_ACTION_INVITE;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

#define MAX_NAME_SIZE 15

struct EndGameResult : public ServerToClientPackageHdr
{
	char m_winner[MAX_NAME_SIZE];

	EndGameResult()
	{
		code = END_GAME_RESULT;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

struct EndGame : public ServerToClientPackageHdr
{
	EndGame()
	{
		code = END_GAME;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

struct ClientToServerPackageHdr : public PackageHdr
{
	enum ClntCommandCode
	{
		CONNECT = 0,
		GET_TOUR_INFO = 1,
		SELECT_TOUR = 2,
		CREATE_TOUR = 3,
		GET_TOUR_PLAYERS = 4,
		ERROR_INFO = 5,
		KEEP_ALIVE = 6,
		GAME_TABLE_ACTION = 7,
		GET_TOURS = 8
	};
};

struct Connect : public ClientToServerPackageHdr
{
	uint8_t m_clientType;
	char m_name[MAX_NAME_SIZE];

	Connect( uint8_t clientType, const char* name ) :
			m_clientType( clientType ), m_name()
	{
		uint32_t bufSize = strlen( name );
		if (bufSize >= MAX_NAME_SIZE)
			bufSize = MAX_NAME_SIZE;
		memcpy( (char*) m_name, (char*) name, bufSize );
		code = CONNECT;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

struct GetTourInfo : public ClientToServerPackageHdr
{
	uint32_t m_tourID;

	GetTourInfo( uint32_t tourID ) :
			m_tourID( tourID )
	{
		code = GET_TOUR_INFO;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

struct SelectTour : public ClientToServerPackageHdr
{
	uint32_t m_tourID;

	SelectTour( uint32_t tourID ) :
			m_tourID( tourID )
	{
		code = SELECT_TOUR;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

struct CreateTour : public ClientToServerPackageHdr
{
	uint32_t m_waitTime;
	uint32_t m_maxPlayers;

	CreateTour( uint32_t waitTime, uint32_t maxPlayers ) :
			m_waitTime( waitTime ), m_maxPlayers( maxPlayers )
	{
		code = CREATE_TOUR;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

struct GetTourPlayers : public ClientToServerPackageHdr
{
	uint32_t m_tourID;

	GetTourPlayers( uint32_t tourID ) :
			m_tourID( tourID )
	{
		code = GET_TOUR_PLAYERS;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

struct ErrorFromClient : public ClientToServerPackageHdr
{
	uint8_t m_errorMessage[0];

	ErrorFromClient()
	{
		code = ERROR_INFO;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

struct KeepAlive : public ClientToServerPackageHdr
{
	KeepAlive()
	{
		code = KEEP_ALIVE;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

struct GameTableAction : public ClientToServerPackageHdr
{
	enum GameActions
		: uint8_t {
			UNDEFINED = 0, SB = 1, BB = 2, CALL = 3, RISE = 4, FALD = 5
	};

	uint8_t m_actionID;
	uint32_t m_ammount;

	GameTableAction( uint32_t actionID, uint32_t ammount = 0 ) :
			m_actionID( actionID ), m_ammount( ammount )
	{
		code = GAME_TABLE_ACTION;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

struct GetTours : public ClientToServerPackageHdr
{
	GetTours()
	{
		code = GET_TOURS;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

#pragma pack(pop)

#endif /* PACKAGESTRUCTURE_HPP_ */
