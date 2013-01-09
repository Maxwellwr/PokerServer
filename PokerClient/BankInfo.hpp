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

#ifndef BANK_INFO_HPP_
#define BANK_INFO_HPP_

struct BankInfo
{
	uint32_t mainBank_;
	uint8_t sideBanksCount_;
	uint32_t *sideBanks_;

	BankInfo() : mainBank_(-1), sideBanksCount_(-1), sideBanks_()
	{

	}
};

#endif /* BANK_INFO_HPP_ */
