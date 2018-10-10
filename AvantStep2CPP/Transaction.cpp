#include "Transaction.h"



CTransaction::CTransaction(double value, time_t time, TransactionType type) : mValue(value), mTime(time), mType(type)
{
}

CTransaction::~CTransaction()
{
}

time_t CTransaction::GetTime()
{
	return this->mTime;
}

CTransaction::TransactionType CTransaction::GetType()
{
	return this->mType;
}

double CTransaction::GetValue()
{
	return this->mValue;
}