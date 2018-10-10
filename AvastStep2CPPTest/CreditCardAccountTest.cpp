#include "stdafx.h"
#include "CppUnitTest.h"
#include "CreditCardAccount.h"
#include "TimeHelper.h"
#include <memory>
#include <ctime>
#include <iostream>
const time_t DEFAULT_TIME = (time_t)1330300800;
const double DEFAULT_APR = 0.35;
const double DEFAULT_CREDIT_LIMIT = 1000.0;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

typedef std::shared_ptr<CCreditCardAccount> CCA;

namespace AvastStep2CPPTest
{
	TEST_CLASS(CreditCardAccountTest)
	{
	public:
		
		CCA EmptyCCA()
		{
			return std::make_shared<CCreditCardAccount>(DEFAULT_APR, DEFAULT_CREDIT_LIMIT, DEFAULT_TIME);
		}

		TEST_METHOD(TestCCAccountContructor)
		{
			CCA cca = this->EmptyCCA();

			// Testing initial values.
			Assert::IsTrue(cca->GetAPR() == DEFAULT_APR, L"The contructor didn't set the apr properly");
			Assert::IsTrue(cca->GetCreditLimit() == DEFAULT_CREDIT_LIMIT, L"The contructor didn't set the credit limit properly");
			Assert::IsTrue(cca->GetStartDate() == DEFAULT_TIME, L"The contructor didn't set the start date properly");
			Assert::IsTrue(cca->GetTransactionCount() == 0, L"The tranasction array should not already have values in it");
		}


		TEST_METHOD(TestCCAccountAddItems1)
		{
			CCA cca = this->EmptyCCA();
			time_t balanceTime = -1;
			// Adding transactions that should work.
			bool result = cca->AddCharge(500.0, 0);
			Assert::IsTrue(result, L"This transaction should have gone through");
			Assert::AreEqual(cca->GetBalanceOnDay(30), 514.38, 0.005, L"Your balance calculation is wrong");

		}

		
		TEST_METHOD(TestCCAccountAddItems2)
		{
			CCA cca = this->EmptyCCA();
			time_t balanceTime = -1;
			
			cca->AddCharge(500.0, 0);
			Assert::IsTrue(cca->AddPayment(200.0, 15), L"This payment should have gone through");
			Assert::AreEqual(cca->GetCurrentBalance(&balanceTime), 300.0, 0.005, L"Your balance calculation after adding a transaction is wrong");
			Assert::IsTrue(balanceTime == (CTimeHelper::AddDays(&DEFAULT_TIME, 15)), L"The time of the current balance calculation is wrong.");
			
			Assert::IsTrue(cca->AddCharge(100.00, 25), L"This charge should have gone through");
			Assert::AreEqual(cca->GetCurrentBalance(&balanceTime), 400.0, 0.005, L"Your balance calculation after adding a transaction is wrong");
			Assert::IsTrue(balanceTime == (CTimeHelper::AddDays(&DEFAULT_TIME, 25)), L"The time of the current balance calculation is wrong.");

			Assert::AreEqual(cca->GetBalanceOnDay(30), 411.99, 0.005, L"Your balance calculation is wrong");

			
		}

		TEST_METHOD(TestCCAccountAddItems3)
		{
			CCA cca = this->EmptyCCA();
			time_t balanceTime = -1;
			
			cca->AddCharge(500.0, 0);
			Assert::IsTrue(cca->AddCharge(200.00, 8), L"This charge should have gone through");
			Assert::AreEqual(cca->GetCurrentBalance(&balanceTime), 700.0, 0.005, L"Your balance calculation after adding a transaction is wrong");
			Assert::IsTrue(balanceTime == (CTimeHelper::AddDays(&DEFAULT_TIME, 8)), L"The time of the current balance calculation is wrong.");


			Assert::IsTrue(cca->AddPayment(200.0, 15), L"This payment should have gone through");
			Assert::AreEqual(cca->GetCurrentBalance(&balanceTime), 500.0, 0.005, L"Your balance calculation after adding a transaction is wrong");
			Assert::IsTrue(balanceTime == (CTimeHelper::AddDays(&DEFAULT_TIME, 15)), L"The time of the current balance calculation is wrong.");

			Assert::IsTrue(cca->AddCharge(100.00, 25), L"This charge should have gone through");
			Assert::AreEqual(cca->GetCurrentBalance(&balanceTime), 600.0, 0.005, L"Your balance calculation after adding a transaction is wrong");
			Assert::IsTrue(balanceTime == (CTimeHelper::AddDays(&DEFAULT_TIME, 25)), L"The time of the current balance calculation is wrong.");

			Assert::AreEqual(cca->GetBalanceOnDay(30), 616.21, 0.005, L"Your balance calculation is wrong");


		}


		TEST_METHOD(TestCCAccountAddItemsCycle2)
		{
			CCA cca = this->EmptyCCA();
			time_t balanceTime = -1;

			// Opening day + cycle 1
			// Identical to TEST_METHOD TestCCAccountAddItems3
			cca->AddCharge(500.0, 0);
			cca->AddCharge(200, 8);
			cca->AddPayment(200, 15);
			cca->AddCharge(100, 25);

			// Cycle 2.
			Assert::IsTrue(cca->AddCharge(300, 35), L"The transactions for cycle 1 went through but not the one for cycle 2.");
			Assert::AreEqual(cca->GetBalanceOnDay(60), 941.12, 0.005, L"Your balance calculation is wrong");


		}

		TEST_METHOD(TestCCAccountAddItemsSkipCycle)
		{
			CCA cca = this->EmptyCCA();
			time_t balanceTime = -1;

			// Opening day + cycle 1
			// Identical to TEST_METHOD TestCCAccountAddItems3
			cca->AddCharge(500.0, 0);
			cca->AddCharge(200, 8);
			cca->AddPayment(200, 15);
			cca->AddCharge(100, 25);

			// Cycle 3.
			Assert::IsTrue(cca->AddCharge(300, 65), L"The transactions for cycle 1 went through but not the one for cycle 2.");
			Assert::AreEqual(cca->GetBalanceOnDay(90), 959.36, 0.005, L"Your balance calculation is wrong");


		}

		/**
		 * Devoted for cases where adding a transaction should fail.
		 */
		TEST_METHOD(TestCCInvalidTransactions)
		{
			CCA cca = this->EmptyCCA();
			time_t balanceTime = -1;

			
		
			bool result = cca->AddCharge(500.0, 0);
			
			// A payment that puts the account balance negative
			result = cca->AddPayment(600, 0);
			Assert::IsFalse(result, L"This payment should have put the balance to -100");
			Assert::AreEqual(cca->GetCurrentBalance(&balanceTime), 500.0, 0.005, L"Your balance calculation after adding a transaction is wrong");
			Assert::IsTrue(balanceTime == (CTimeHelper::AddDays(&DEFAULT_TIME, 0)), L"The time of the current balance calculation is wrong.");
			result = cca->AddPayment(1600, 20);
			Assert::IsFalse(result, L"This payment should have put the balance to -1100");
			Assert::AreEqual(cca->GetCurrentBalance(&balanceTime), 500.0, 0.005, L"Your balance calculation after adding a transaction is wrong");
			Assert::IsTrue(balanceTime == (CTimeHelper::AddDays(&DEFAULT_TIME, 0)), L"The time of the current balance calculation is wrong.");



			// A charge that would put the person above the credit limit
			result = cca->AddCharge(600, 1);
			Assert::IsFalse(result, L"This payment should have put the balance to 1100, more than credit limit 1000");
			Assert::AreEqual(cca->GetCurrentBalance(&balanceTime), 500.0, 0.005, L"Your balance calculation after adding a transaction is wrong");
			Assert::IsTrue(balanceTime == (CTimeHelper::AddDays(&DEFAULT_TIME, 0)), L"The time of the current balance calculation is wrong.");

			result = cca->AddCharge(700, 0);
			Assert::IsFalse(result, L"This payment should have put the balance to 1200, more than credit limit 1000");
			Assert::AreEqual(cca->GetCurrentBalance(&balanceTime), 500.0, 0.005, L"Your balance calculation after adding a transaction is wrong");
			Assert::IsTrue(balanceTime == (CTimeHelper::AddDays(&DEFAULT_TIME, 0)), L"The time of the current balance calculation is wrong.");

			
			// Tricky case, say we have a balance of $960 by day 8 and do no payments. The interest should mean that I can't charge $40 on
			// day 30.

			cca->AddCharge(460, 8);
			result = cca->AddCharge(40, 31);
			Assert::IsFalse(result, L"This payment should have put the balance to 1024.09, more than credit limit 1000");
			Assert::AreEqual(cca->GetCurrentBalance(&balanceTime), 960.0, 0.005, L"Your balance calculation after adding a transaction is wrong");
			Assert::IsTrue(balanceTime == (CTimeHelper::AddDays(&DEFAULT_TIME, 8)), L"The time of the current balance calculation is wrong.");


		}


		TEST_METHOD(TestCCTransactionOrder)
		{
			CCA cca = this->EmptyCCA();
			time_t balanceTime = -1;



			bool result = cca->AddCharge(500.0, 0);
			Assert::IsTrue(result, L"This transaction should have gone through");

			result = cca->AddPayment(500.0, 0);
			Assert::IsTrue(result, L"This transaction should have gone through");

		}
	};
}