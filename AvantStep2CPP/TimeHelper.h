#pragma once
#include <ctime>


/**
 * Class dedicated to enforcing the use of times that are in GMT format.
 * Provides helpful functions related to time used by other classes.
 */
class CTimeHelper
{
public:
	static time_t mktimeGMT(struct tm * const time);
	static time_t GetEndOfDay(const time_t time);
	static time_t GetStartOfDay(const time_t time);
	static time_t AddDays(const time_t * time, int days);
	static int DiffDays(const time_t time1, const time_t time2);
	static int DiffDays(struct tm * const t1, struct tm * const t2);
	CTimeHelper() = delete;
	~CTimeHelper();
};

