#include "CreditCardAccount.h"
#include <ctime>
#include <algorithm>
#include "TimeHelper.h"
#include "TransactionFactory.h"
using std::vector;
using std::shared_ptr;
using std::find_if;

/// The default time an account is opened. It is midnight on February 27, 2012 GMT;
const time_t DEFAULT_TIME = (time_t)1330300800;

/// The amount of days in one cycle. 
const int DAYS_PER_CYCLE = 30;


/**
* Constructor.
* \param startTime The time the credit card account which will use this was started at.
* \param cycle The cycle we are determining if the transactions occurred inside.
*/
CCreditCardAccount::IsInCycle::IsInCycle(const time_t startTime, const int cycle) : _startTime(startTime), _cycle(cycle)
{
}

/**
 * Predicate operation
 * \param transaction The transaction we are evaluating
 * \returns True if the transaction occured during the cycle stored in this functor.
 */
bool CCreditCardAccount::IsInCycle::operator()(const std::shared_ptr<CTransaction>& transaction) const
{
	int cycle = CCreditCardAccount::GetCycle(transaction->GetTime(), this->_startTime);
	return (this->_cycle == cycle);
}

/**
 * Constructor.
 * \param startTime The time the credit card account which will use this was started at. 
 * \param day The day we are comparing the transactions' times to.
 */
CCreditCardAccount::IsPastDay::IsPastDay(const time_t startTime, const int day) : _startTime(startTime), _day(day)
{
}

/**
 * 
 * \param transaction The transaction we are evaluating.
 * \returns True if the transaction occurred past the day stored in this functor.
 */
bool CCreditCardAccount::IsPastDay::operator()(const std::shared_ptr<CTransaction>& transaction) const
{
	int day = CTimeHelper::DiffDays(transaction->GetTime(), this->_startTime);
	return (day > this->_day);
}


/**
* Constructor.
* \param startTime The time the credit card account which will use this was started at.
* \param cycle The cycle we are determining if the transactions occurred inside or past.
*/
CCreditCardAccount::IsInOrPastCycle::IsInOrPastCycle(const time_t startTime, const int cycle): IsInCycle(startTime, cycle)
{
}

/**
* Predicate operation
* \param transaction The transaction we are evaluating
* \returns True if the transaction occured during or after the cycle stored in this functor.
*/
bool CCreditCardAccount::IsInOrPastCycle::operator()(const std::shared_ptr<CTransaction>& transaction) const
{
	int cycle = CCreditCardAccount::GetCycle(transaction->GetTime(), this->_startTime);
	return (this->_cycle <= cycle);
}



/**
 * Constructor.
 * \param apr The APR of the credit card.
 * \param limit The limit on the account balance.
 * \param startDate The day and time the account was started at.
 */
CCreditCardAccount::CCreditCardAccount(double apr, double limit, time_t startDate = DEFAULT_TIME): mAPR(apr), mCreditLimit(limit), mStartDate(startDate)
{
	this->mTransactions = TransactionVector();
}


/**
 * Destructor.
 */
CCreditCardAccount::~CCreditCardAccount()
{
	this->mTransactions.clear();
}

/**
 * Get the cycle the given time would occur during based on the time the account started
 * \param currentTime Time for whcih we are trying to find which cycle it would occur in.
 * \param startTime Time the account was started.
 * \returns The cycle that the given time would occur during it. 
 */
int CCreditCardAccount::GetCycle(time_t currentTime, time_t startTime)
{
	int diffDays = CTimeHelper::DiffDays(currentTime, startTime);
	return diffDays / DAYS_PER_CYCLE;
}


/**
 * Get the cycle the given transaction would occur during based on the time the account started
 * \param transaction Transaction for whcih we are trying to find which cycle it would occur in.
 * \param startTime Time the account was started.
 * \returns The cycle that the transaction would occur during it
 */
int CCreditCardAccount::GetCycle(TransactionIter transaction, time_t startTime)
{
	time_t time = transaction->get()->GetTime();
	return GetCycle(time, startTime);
}


/**
* Get the day within the cycle (0-29) the given transaction would occur during based on the time the account started
* \param transaction time to find when in a cycle it would occure.
* \param startTime Time the account was started.
* \returns The day in the cycle that the given time would occur during it
*/
int CCreditCardAccount::DayInCycle(time_t currentTime, time_t startTime)
{
	int diffDays = CTimeHelper::DiffDays(currentTime, startTime);

	return diffDays % DAYS_PER_CYCLE;

}

/**
 * Get the day that the transaction occurred on based on the starting time of the account.
 * \param transaction The transaction for which we are trying to find the day of.
 * \param startTime The time the account was started.
 * \returns The day in the account's history the transaction occured on. 
 */
int CCreditCardAccount::GetDayOfTransaction(TransactionIter transaction, time_t startTime)
{
	time_t time = transaction->get()->GetTime();
	return CTimeHelper::DiffDays(time, startTime);
}

/**
 * Get the first transaction that occurred in or after the given cycle.
 * \param cycle The given cycle. The return transaction will come on or after midnight of the first day of the cycle.  
 * \returns The first transaction that occurred in or after the given cycle. Or end() if no such transaction exists. 
 */
TransactionIter CCreditCardAccount::FirstTransactionAfterCycleStart(int cycle)
{
	if (cycle < this->GetCycleCount())
	{
		TransactionIter iter = find_if(this->mTransactions.begin(), this->mTransactions.end(), CCreditCardAccount::IsInOrPastCycle(this->mStartDate, cycle));
		return iter;
	}
	return this->mTransactions.end();
}

/**
* Get's the start of a cycle's iterators of transactions. Think of it like vector.begin()
* \param cycle The cycle that the transaction should be the first of.
* \returns Iterator to the first transaction in the cycle or an iterator to the end of the vector
*		if there are no transactions in that cycle.
*/
TransactionIter CCreditCardAccount::CycleBegin(int cycle)
{
	if (cycle < this->GetCycleCount())
	{

		TransactionIter iter = find_if(this->mTransactions.begin(), this->mTransactions.end(), CCreditCardAccount::IsInCycle(this->mStartDate, cycle));
		return iter;
	}
	return this->mTransactions.end();
}

/**
* Get's the end of a cycle's iterators of transactions. Think of it like vector.end()
* \param cycle The cycle that the transaction should be closest to the end of.
* \returns Iterator immediately after the last one in the cycle or an iterator to the end of the vector
*		if there are no transactions in that cycle.
*/
TransactionIter CCreditCardAccount::CycleEnd(int cycle)
{
	TransactionIter cycleStart = this->CycleBegin(cycle);
	TransactionIter searchEnd = this->FirstTransactionAfterCycleStart(cycle + 1);
	for (; searchEnd != cycleStart; --searchEnd)
	{
		if (GetCycle((searchEnd - 1), this->mStartDate) == cycle)
		{
			return searchEnd;
		}
	}

	return cycleStart;

	/**TransactionIter nextCycleStart = find_if(cycleStart, this->mTransactions.end(), CCreditCardAccount::IsInCycle(this->mStartDate, cycle + 1));
	return --nextCycleStart;*/
}

/**
 * Get's the most recent transaction that occurred on or before that day.
 * \param day The day that the transaction should be closest to the end of.
 * \returns Iterator most recent transaction on or before that day.
 */
TransactionIter CCreditCardAccount::LastTransactionOfDay(int day)
{
	if (GetDayOfTransaction(this->mTransactions.begin(), this->mStartDate) > day)
	{
		return this->mTransactions.end();
	}
	return --(find_if(this->mTransactions.begin(), mTransactions.end(), CCreditCardAccount::IsPastDay(this->mStartDate, day)));
}



/**
 * Create a new transaction. Add it to the vector of transactions.
 * \param value The value of the transaction
 * \param day How many days after the opening of the account the transaction occurred.
 * \param type The type of transaction. Charges increase balance, payments decrease it.
 * \returns True if the addition was successful. False otherwise. 
 */
bool CCreditCardAccount::AddTransaction(double value, int day, CTransaction::TransactionType type)
{
	CTransactionFactory factory = CTransactionFactory();
	shared_ptr<CTransaction> transaction = factory.CreateTransaction(value, &(this->mStartDate), day, type);

	time_t transactionTime = transaction->GetTime();

	TransactionIter insertIter = mTransactions.end();
	// Quick shortcut that makes this function O(1) in most cases.
	if (!(this->mTransactions.empty()) && this->mTransactions.back()->GetTime() <= transactionTime)
	{
		// This would truly be the most recent transaction. No need to go through the loop.
	} 
	else
	{
		// We have to find where in the collection this transaction belongs. The list should stay in order by the 
		// time of the transaction.
		for (TransactionIter iter = this->mTransactions.begin(); iter != this->mTransactions.end(); ++iter)
		{
			time_t iterTime = iter->get()->GetTime();
			if (transactionTime < iterTime) {
				insertIter = iter;
				iter = --(this->mTransactions.end());
			}
		}
	}


	// Next we're going to figure out what the balance would after the time we add this transaction if we 
	// were to add it. This makes sure we don't do any invalid transactions.
	int cycle = GetCycle(transaction->GetTime(), this->mStartDate);
	double balance = 0.0;
	if (insertIter == this->mTransactions.end())
	{
		balance = mBalance;

		// If this transaction is the newest chronologically then we can take a shortcut in calculating
		// the balance after it is added, by just accumulating any interest that occurred before it was made,
		// and then finding the balance change from the actual transaction itself. 
		if (this->GetTransactionCount() > 0)
		{
			// We're going to see if any cycles were completed by adding this transaction.
			TransactionIter lastTransaction = insertIter - 1;
			int currentCycleCount = this->GetCycleCount() - 1;
			if (cycle - currentCycleCount > 0)
			{
					// Counterintuitively, we have to remove the transactions in the last cycle from the balance for this to work. 
					// We need to get the first one in the cycle first. 
					TransactionIter cycleStart = this->CycleBegin(currentCycleCount);

				for (TransactionIter iter = cycleStart; iter != this->mTransactions.end(); ++iter)
				{
					switch (iter->get()->GetType())
					{
					case CTransaction::CHARGE:
						balance -= transaction->GetValue();
						break;
					case CTransaction::PAYMENT:
						balance += transaction->GetValue();
						break;
					default:
						break;
					}
				}




				balance = this->CalculateInRange(balance, cycleStart, this->mTransactions.end(), cycle);
			}

		}
		TransactionIter addedIter = this->mTransactions.insert(insertIter, transaction);

		balance = this->CalculateInRange(balance, addedIter, this->mTransactions.end(), cycle);
		if (balance - this->mCreditLimit > 0.000001 || balance < 0.0)
		{
			// Ading this transaction either puts the balance above the limit or puts it to negative. 
			// Either way, delete it and return that the adding was unsuccessful.
			this->mTransactions.erase(addedIter);
			return false;
		}
		else
		{
			// Adding this transaction can be done successfully.
			this->mBalance = balance;
			this->mBalanceDate = transaction->GetTime();
			return true;
		}
	}
	else
	{
		// Since we also have the power to add a transaction in the middle of the history, we
		// need to calculate the balance from scatch.
		TransactionIter addedIter = this->mTransactions.insert(insertIter, transaction);
		balance = this->CalculateInRange(this->mBalance, this->mTransactions.begin(), this->mTransactions.end(), cycle);

		if (balance - this->mCreditLimit > 0.000001 || balance < 0.0)
		{
			// For right now I made it impossible for adding a transaction in the middle to fail.
			// It seems to me the equivalent of canceling the compounding of interest if it made the 
			// the balance over the credit limit. This isn't rejecting a charge or payment. The only
			// reason to ever add a transaction in the middle would be an error on fault of the 
			// credit card company.
			this->mBalance = balance;
			this->mBalanceDate = transaction->GetTime();
			return true;
		}
		else
		{
			// Adding this transaction can be done successfully.
			this->mBalance = balance;
			this->mBalanceDate = transaction->GetTime();
			return true;
		}

	}
	
}



/**
 * Get the interest that would occur and the end of the day.
 * \param balance The balance at the end of the day.
 * \returns The interest if the given balance was at the end of the day.
 */
double CCreditCardAccount::GetEndDayInterest(double balance)
{
	// Daily Interest calculation from the instruction email. 
	return balance * this->mAPR / 365;
}


/**
 * Heart valve of the balance calculation. Does the calculation over a cycle. Applies interest. 
 * You have to give it the iterator to the first transaction in the cycle and to the 
 * one immediately after the last transaction in the cycle.
 * \param balance Balance before the cycle begins.
 * \param start Iterator irst transaction in the cycle.
 * \param end Iterator DIRECTLY AFTER the last transaction in the cycle.
 * \param justInterest. If true only return the interest of the cycle.
 * \returns The balance after the cycle, with interest applied, unless justInterest is true. In that case, you'll only get the interest.
 */
double CCreditCardAccount::CalculateCycle(double balance, TransactionIter start, TransactionIter end, bool justInterest = false)
{

	double interest = 0.0;
	int prevDayInCycle = 0;
	for (; start != end; ++start)
	{
		CTransaction *transaction = start->get();
	
		// Get the interest acculumated between this transaction and the previous transaction. 
		int dayInCycle = DayInCycle(transaction->GetTime(), this->mStartDate);
		interest += this->GetEndDayInterest(balance) * (double)(dayInCycle - prevDayInCycle);
		prevDayInCycle = dayInCycle;

		// Apply this transaction to the balance.
		switch (transaction->GetType())
		{
		case CTransaction::CHARGE:
			balance += transaction->GetValue();
			break;
		case CTransaction::PAYMENT:
			balance -= transaction->GetValue();
			break;
		default:
			break;
		}
	}

	// Get the interest accululated between the last transaction in the cycle and the end of the cycle.
	int daysLeftInCycle = DAYS_PER_CYCLE - (prevDayInCycle);
	interest += this->GetEndDayInterest(balance) * (double)(daysLeftInCycle);

	if (justInterest)
	{
		return interest;
	}

	return balance + interest;
}


/**
 * The heart of the balance calculation. Generates a balance based on: an initial balance before the calculation,
 * the first transaction we are applying in the calculation, the last transaction we are applying in the calculation,
 * and how many cycles we want to have occured in total of the entire account.
 * \param balance The initial balance
 * \param start Iterator to the first transaction we are applying in this calculation. 
 * \param end Iterator that occurs DIRECTLY AFTER the last transaction we want to apply.
 * \param cycleCount How many cycles we want to have occurred in the entire account. For example, if the last
 *		transaction in the entire account occured on day 24, but we want to know the balance on day 30, a 
 *		cycle would have occured in that time, so we would have a cycleCount of 1. 
 * \returns 
 */
double CCreditCardAccount::CalculateInRange(double balance, TransactionIter start, TransactionIter end, int cycleCount)
{
	int prevCycle = GetCycle(start, this->mStartDate);

	while (start != this->mTransactions.end() && start != end)
	{
		int cycle = GetCycle(start, this->mStartDate);
		
		// This only happens if a cycle was skipped between transactions. 
		// We need to collect the interest in these skipped cycles.
		while (cycle - prevCycle > 1)
		{
			balance += this->GetEndDayInterest(balance) * DAYS_PER_CYCLE;
			prevCycle++;
		}

		
		TransactionIter cycleEnd = this->CycleEnd(cycle);
		if (cycleEnd != mTransactions.end())
		{
			balance = this->CalculateCycle(balance, start, cycleEnd);
			start = cycleEnd;
		} 
		else
		{
			// These are the last transactions in the range. They don't
			// form a complete cycle unless you explicitly say so using
			// the cycleCount variable.

			// Even if they are the last transactions, if cycles expected from
			// the day we are asking the balance for is past the cycle of these
			// last transactions. That means we have to treat these as if they were
			// a complete cycle. 
			if (cycle < cycleCount)
			{
				balance = this->CalculateCycle(balance, start, this->mTransactions.end());
				start = this->mTransactions.end();
				cycle++;
			}
			else
			{
				// When we asked for the balance of this range, we asked for the balance
				// on a day within the cycle these last transactions are a part of.
				// We don't have to care about interest at all.
				for (; start != this->mTransactions.end(); ++start)
				{
					CTransaction *transaction = start->get();

					// Apply this transaction to the balance.
					switch (transaction->GetType())
					{
					case CTransaction::CHARGE:
						balance += transaction->GetValue();
						break;
					case CTransaction::PAYMENT:
						balance -= transaction->GetValue();
						break;
					default:
						break;
					}
				}
			}
		}
		prevCycle = cycle;
	}
	

	// Say we ask for the balance somewhere between the 5th and 6th cycle but our last transaction was before the end of the 
	// 3rd. We would have to apply the interest at the end of the 3rd, 4th, and 5th cycle. That's what we do here.
	// You have to do this in a loop (not all at once), since interest compounds. This could use exponential math in the future. 
	// However, if you look above, you'll see we have to apply the interest on the 3rd cycle on a day by day basis (such as
	// we would have to for the 1st and 2nd cycle in the scenario). Cycles 4 and 5 would be calculated much more easily since
	// no transactions were made during them.
	while (prevCycle < cycleCount)
	{
		balance += this->GetEndDayInterest(balance) * DAYS_PER_CYCLE;
		prevCycle++;
	}

	return balance;
}

/**
 * Get the cycle in which the most recent transaction occurs during.
 * \returns The cycle of the most recent transaction.
 */
int CCreditCardAccount::GetCycleCount()
{
	if (this->mTransactions.empty())
	{
		return 0;
	}
	TransactionIter lastTrans = --(this->mTransactions.end());
	return GetCycle(lastTrans, this->mStartDate) + 1;
}

/**
 * Get the total number of transactions that have occured in this 
 * account.
 * \returns The number of transactions in the container that keeps track of them. 
 */
size_t CCreditCardAccount::GetTransactionCount()
{
	return this->mTransactions.size();
}


/**
 * Get the interest rate of the account.
 * \returns The APR as a decimal (.10 = 10%).
 */
double CCreditCardAccount::GetAPR()
{
	return this->mAPR;
}

/**
 * Get the maximum balance the account can have before no more money can be charged.
 * \returns The limit of the credit card.
 */
double CCreditCardAccount::GetCreditLimit()
{
	return this->mCreditLimit;
}

/**
 * Get the date and time the account was started.
 * \returns The date and time the account was started.
 */
time_t CCreditCardAccount::GetStartDate()
{
	return this->mStartDate;
}

/**
 * Gets the balance of the card up to the time of the most recent transaction.
 * \param transactionTime The time of the most recent transaction will be stored in this.
 * \returns The balance of the account as of the last transaction addition.
 */
double CCreditCardAccount::GetCurrentBalance(time_t * transactionTime = nullptr)
{
	if (transactionTime != nullptr)
	{
		*transactionTime = this->mBalanceDate;
	}
	return this->mBalance;
}


/**
 * Add a payment transaction. Decreases balance.
 * \param value The value of the payment.
 * \param day The day relative to the account opening day that the payment occurred. 0 is opening day.
 * \returns True if successful. False otherwise. Most likely reason for a failure is if the payment would put the account in negative.
 */
bool CCreditCardAccount::AddPayment(double value, int day)
{
	return this->AddTransaction(value, day, CTransaction::PAYMENT);
}

/**
* Add a charge transaction. Increases balance.
* \param value The value of the charge.
* \param day The day relative to the account opening day that the charge occurred. 0 is opening day.
* \returns True if successful. False otherwise. Most likely reason for a failure is if the charge would put the account over the credit limit.
*/
bool CCreditCardAccount::AddCharge(double value, int day)
{
	return this->AddTransaction(value, day, CTransaction::CHARGE);
}


/**
 * Get what the balance would be on a specific day.
 * \param day The day we want to get the balance on.
 * \returns The balance on that day.
 */
double CCreditCardAccount::GetBalanceOnDay(int day)
{
	time_t timeOfDay = CTimeHelper::AddDays(&this->mStartDate, day);
	int cycleOfDay = GetCycle(timeOfDay, this->mStartDate);

	// This isn't the same as the last calculated balance member variable!!
	double startingBalance = 0.0;

	TransactionIter lastOfDay = this->LastTransactionOfDay(day);
	if (lastOfDay == this->mTransactions.end())

	{
		// There are no transactions to apply between the creation of the account and the day we are asking the balance 
		// at the end of. So the balance should be zero. However, if for some reason you want to make them start with a 
		// balance greater than zero, this line will apply the interest over the cycles that complete before the given day.
		return this->CalculateInRange(startingBalance, this->mTransactions.end(), this->mTransactions.end(), cycleOfDay);
	}
	else
	{
		return this->CalculateInRange(startingBalance, this->mTransactions.begin(), ++lastOfDay, cycleOfDay);
	}
}