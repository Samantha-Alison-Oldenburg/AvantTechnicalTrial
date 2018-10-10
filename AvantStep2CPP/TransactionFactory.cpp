/**
 * \file TransactionFactory.cpp
 *
 * \author Samantha Oldenburg
 */

#include "TransactionFactory.h"
#include "TimeHelper.h"

using std::shared_ptr;


/**
 * Constructor. Doesn't do much.
 */
CTransactionFactory::CTransactionFactory()
{
}


/**
 * Destructor.
 */
CTransactionFactory::~CTransactionFactory()
{
}



/**
 * Create a new CTransaction object.
 * \param value The value of the transaction.
 * \param time The time the transaction took place as a time struct.
 * \param type The type of transaction. Charges increase balance, payments decrease it.
 * \returns A shared pointer to the created CTransaction object.
 */
shared_ptr<CTransaction> CTransactionFactory::CreateTransaction(double value, tm * time, CTransaction::TransactionType type)
{
	// Convert to epoch time
	time_t transactionTime = CTimeHelper::mktimeGMT(time);

	return std::make_shared<CTransaction>(value, transactionTime, type);
}


/**
 * Create a new CTransaction object. A more simple version that
 * assumes when a transaction happens during a day doesn't matter.
 * It will simply give the transaction a time that occurs {days} days
 * after the day the account opened.
 * \param value The value of the transaction.
 * \param accountStart The day the credit card account was started.
 * \param days How many days after the account 
 * \param type The type of transaction. Charges increase balance, payments decrease it.
 * \returns A shared pointer to the created CTransaction object.
 */
shared_ptr<CTransaction> CTransactionFactory::CreateTransaction(double value, const time_t * accountStart, int days, CTransaction::TransactionType type)
{
	// Convert epoch time of the account start time to a tm struct. We can add days to this struct without fear of overlap.
	struct tm *time = gmtime(accountStart);

	// Our days start at index 0 so we have to add one.
	time->tm_mday += days;

	// Prevent ambiguity with the function that converts to a time_h object
	time->tm_yday = 0;


	return CreateTransaction(value, time, type);
}