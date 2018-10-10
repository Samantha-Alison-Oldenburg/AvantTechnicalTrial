#pragma once
#include "Transaction.h"
#include <memory>


/**
 * A class that is used to create transactions to be added. It deals with all the math
 * to turn the days since the account has opened into a time variable so that the 
 * CTransaction and CCreditCardAccount classes don't have to.
 */
class CTransactionFactory
{
public:
	CTransactionFactory();
	virtual ~CTransactionFactory();

	std::shared_ptr<CTransaction> CreateTransaction(double value, struct tm * time, CTransaction::TransactionType type);
	std::shared_ptr<CTransaction> CreateTransaction(double value, const time_t *accountStart, int days, CTransaction::TransactionType type);

};

