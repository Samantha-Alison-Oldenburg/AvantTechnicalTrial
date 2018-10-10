#pragma once
#include <ctime>


/**
 * Class representing transactions made. They can either be charges to the card or payments. Either way, the value of the transaction 
 * is positive. Leave it to the credit card to organize purchases and payments.
 */
class CTransaction
{

public:
	/// The type of transaction. A charge means the card was swiped to buy something. It increases the outstanding balance.
	/// A payment is done to the credit card company, it decreases the outstanding balance.
	enum TransactionType
	{
		CHARGE,
		PAYMENT
	};

private:		
	/// How much money is in the transaction. Should always be positive. A transaction is a way to acknowledge money was exchanged. You cannot exchange negative money to someone. 
	double mValue;
	
	/// The type of transaction made. See TransactionType for more details.
	TransactionType mType;
	
	/// The time the transaction took place, stored as seconds after unix epoch time. 
	time_t mTime;

public:	
	

	CTransaction() = delete;
	CTransaction(const CTransaction &) = delete;
	CTransaction(double value, time_t time, TransactionType type);

	time_t GetTime();
	TransactionType GetType();
	double GetValue();

	virtual ~CTransaction();
};

