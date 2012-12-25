/*
 * Bank.hpp
 *
 *  Created on: 26.12.2012
 *      Author: Alexey
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
