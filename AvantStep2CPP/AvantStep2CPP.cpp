// AvantStep2CPP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <ctime>
#include "TimeHelper.h"
#include "CreditCardAccount.h"
#include <memory>
#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::cin;
using std::string;

const time_t DEFAULT_TIME = (time_t)1330300800;
const double DEFAULT_APR = 0.35;
const double DEFAULT_CREDIT_LIMIT = 1000.0;


typedef std::shared_ptr<CCreditCardAccount> CCA;

void print_help_prompt()
{
	cout << "Hello!" << endl;
	cout << "p\tAdd a payment" << endl;
	cout << "c\tAdd a charge" << endl;
	cout << "b\tGet a balance" << endl;
	cout << "h\tDisplay this help message" << endl;
	cout << "q\tQuit" << endl;
}


int main()
{
	tzset();
	double apr, creditLimit;
	cout << "APR (as decimal)? ";
	cin >> apr;
	cout << "Credit Limit? ";
	cin >> creditLimit;
	CCA cca = std::make_shared<CCreditCardAccount>(apr, creditLimit, DEFAULT_TIME);
	print_help_prompt();

	bool notQuit = true;
	while (notQuit)
	{
		char command;
		cin >> command;
		switch (command)
		{
		case 'p':
		{
			double value;
			int day;
			cout << "How much was the payment? ";
			cin >> value;
			cout << "How many days after the account opening did it happen? ";
			cin >> day;

			if (cca->AddPayment(value, day))
			{
				cout << "Payment was successful!" << endl;
			}
			else
			{
				cout << "Payment was too much!" << endl;
			}
			break;
		}
		case 'c':
		{
			double value;
			int day;
			cout << "How much was the charge? ";
			cin >> value;
			cout << "How many days after the account opening did it happen? ";
			cin >> day;

			if (cca->AddCharge(value, day))
			{
				cout << "Charge was successful!" << endl;
			}
			else
			{
				cout << "Charge was too much!" << endl;
			}
			break;
		}
		case 'b':
		{
			int day;
			cout << "On what day did you want the balance? ";
			cin >> day;
			cout << "Balance on " << day << " after starting account: " << cca->GetBalanceOnDay(day) << endl;
			break;
		}
		case 'h':
			print_help_prompt();
			break;
		case 'q':
			notQuit = false;
			break;

		}
	}

    return 0;
}

