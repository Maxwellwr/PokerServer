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

#ifndef CONNECTSESSION_H_
#define CONNECTSESSION_H_

#include <memory>
#include "asio.hpp"
#include "Definitions.hpp"

using namespace std;

class ConnectSession
{
public:
	ConnectSession( shared_ptr<asio::io_service> ioService );
	~ConnectSession();

	void reply();
	SOCKET& getSocket();
private:
	shared_ptr<asio::io_service> m_Service;
	SOCKET m_Socket;
};

#endif /* CONNECTSESSION_H_ */
