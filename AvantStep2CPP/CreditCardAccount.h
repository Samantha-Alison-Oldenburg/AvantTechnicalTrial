#pragma once
#include <memory>
#include <ctime>
#include "Transaction.h"
#include <vector>
#include <iterator>

typedef std::vector<std::shared_ptr<CTransaction>> TransactionVector;
typedef TransactionVector::iterator TransactionIter;


/**
 * The only class you should ever instantiate yourself. Represents a credit card account.
 * The card has an APR and Credit Limit. Interest is calculated daily at the close of each day, but not applied.
 * Interest is applied to the balance at the close of each 30-day period (opening day excluded).
 */
class CCreditCardAccount
{
public:

	static int GetCycle(time_t currentTime, time_t startTime);
	static int GetCycle(TransactionIter transaction, time_t startTime);
	static int DayInCycle(time_t currentTime, time_t startTime);
	static int GetDayOfTransaction(TransactionIter transaction, time_t startTime);

private:
	/// Container containing all charges and payments.
	TransactionVector mTransactions;
	
	/// The start date of the account
	time_t mStartDate;

	/// The date of the latest calculation of the account's outstanding balance.
	time_t mBalanceDate = -1;

	/// The outstanding balance of the account, according to the time in mBalanceDate and calculated 
	/// from the current members of mTransactions.
	double mBalance = 0.0;

	/// The APR (interest rate).
	double mAPR = 0.0;

	/// The upper limit of the outstanding balance.
	double mCreditLimit = 0.0;

	TransactionIter FirstTransactionAfterCycleStart(int cycle);
	TransactionIter CycleBegin(int cycle);
	TransactionIter CycleEnd(int cycle);
	TransactionIter LastTransactionOfDay(int day);
	

	bool AddTransaction(double value, int day, CTransaction::TransactionType type);

	double GetEndDayInterest(double balance);

	double CalculateCycle(double balance, TransactionIter start, TransactionIter end, bool justInterest);
	
	double CalculateInRange(double balance, TransactionIter start, TransactionIter end, int cycleCount);
	
	/**
	 * This is a functor class. It is used to determine if a given transaction is
	 * within the cycle you give the object of this class at instantiation. 
	 *
	 * Essentially it is used by the <algorithm> functions as a binary predicate. 
	 * For an example, see std::find_if
	 */
	struct IsInCycle : std::unary_function<std::shared_ptr<CTransaction>, bool>
	{
		IsInCycle(const time_t startTime, const int cycle);
		virtual bool operator() (const std::shared_ptr<CTransaction> & transaction) const;

		const time_t _startTime;
		const int _cycle;
	};

	/**
	* This is a functor class. It is used to determine if a given transaction occurs after the 
	* day that you give the object of this class at instantiation.
	*
	* Essentially it is used by the <algorithm> functions as a binary predicate.
	* For an example, see std::find_if
	*/
	struct IsPastDay : std::unary_function<std::shared_ptr<CTransaction>, bool>
	{
		IsPastDay(const time_t startTime, const int day);
		bool operator() (const std::shared_ptr<CTransaction> & transaction) const;

		const time_t _startTime;
		const int _day;
	};

	/**
	* This is a functor class. It is used to determine if a given transaction occurs after the
	* day that you give the object of this class at instantiation.
	*
	* Essentially it is used by the <algorithm> functions as a binary predicate.
	* For an example, see std::find_if
	*/
	struct IsInOrPastCycle : IsInCycle
	{
		IsInOrPastCycle(const time_t startTime, const int cycle);
		bool operator() (const std::shared_ptr<CTransaction> & transaction) const;

	};


public:
	// Never use the default constructor.
	CCreditCardAccount() = delete;
	// TODO: Implement the copy constructor. For now it is deleted.
	CCreditCardAccount(const CCreditCardAccount &) = delete;
	CCreditCardAccount(double apr, double limit, time_t startDate);
	virtual ~CCreditCardAccount();

	int GetCycleCount();
	size_t GetTransactionCount();
	double GetAPR();
	double GetCreditLimit();
	time_t GetStartDate();

	double GetCurrentBalance(time_t *transactionTime);

	bool AddPayment(double value, int day);
	bool AddCharge(double value, int day);

	
	double GetBalanceOnDay(int day);

	
};

