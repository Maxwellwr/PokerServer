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

struct PackageHdr
{
	uint32_t code;
	uint32_t bodyLength;
};

struct ServerToClientPackageHdr : public PackageHdr
{
	enum ServCommandCode
	{
		CONNECT_OK, TOUR_INFO, SELECT_OK, PLAYERS_INFO
	};
};

template<uint32_t tourCount>
	struct ConnectOk : public ServerToClientPackageHdr
	{
		uint32_t m_toursCount;
		uint32_t m_toursIDs[tourCount];

		ConnectOk( uint32_t* toursID ) :
				m_toursCount( tourCount )
		{
			std::copy( toursID, toursID + m_toursCount, m_toursIDs );
			code = CONNECT_OK;
			bodyLength = sizeof(*this) - sizeof(PackageHdr);
		}
	};

struct TourInfo : public ServerToClientPackageHdr
{
	uint32_t m_tourID;
	uint32_t m_freePlaces;
	uint32_t m_creationTime;
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
	uint32_t m_beginTime;

	SelectOk( uint32_t tourID, uint32_t beginTime ) :
			m_tourID( tourID ), m_beginTime( beginTime )
	{
		code = SELECT_OK;
		bodyLength = sizeof(*this) - sizeof(PackageHdr);
	}
};

template<uint32_t playerCount>
	struct PlayersInfo : public ServerToClientPackageHdr
	{
		uint32_t m_tourID;
		uint32_t m_playersCount;
		char m_playersNames[playerCount];

		PlayersInfo( uint32_t tourID, char* playersNames ) :
				m_tourID( tourID ), m_playersCount( playerCount )
		{
			std::copy( playersNames, playersNames + m_playersCount,
					m_playersNames );
			code = PLAYERS_INFO;
			bodyLength = sizeof(*this) - sizeof(PackageHdr);
		}
	};

struct ClientToServerPackageHdr : public PackageHdr
{
	enum ClntCommandCode
	{
		CONNECT, GET_TOUR_INFO, SELECT_TOUR, CREATE_TOUR, GET_TOUR_PLAYERS
	};
};

struct Connect : public ClientToServerPackageHdr
{
	char m_clientType;
	char m_name[15];

	Connect( char clientType, char* name ) :
			m_clientType( clientType )
	{
		std::strncpy( m_name, name, sizeof(m_name) );
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
	uint32_t m_beginTime;

	CreateTour( uint32_t waitTime, uint32_t beginTime ) :
			m_waitTime( waitTime ), m_beginTime( beginTime )
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

#endif /* PACKAGESTRUCTURE_HPP_ */
