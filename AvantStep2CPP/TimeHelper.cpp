/**
 * \file TimeHelper.cpp
 *
 * \author Samantha Oldenburg
 */


#include "TimeHelper.h"

/// Conversion ratio between days (24 hours) and seconds
const int DAYS_TO_SECS = 60 * 60 * 24;


/**
 * Destructor.
 */
CTimeHelper::~CTimeHelper()
{
}



/**
 * Identical to mktime in the <ctime> library. The only difference is that it 
 * subtracts the timezone offset to ensure GMT format. 
 * \param time The time struct containing the information to generate the unix epoch time
 * \returns The corresponding unix epoch time in GMT format.
 */
time_t CTimeHelper::mktimeGMT(tm * const time)
{
	return mktime(time) - _timezone;
}


/**
 * Get's the time that is the end of the day of the given time (i.e. midnight of the next day).
 * \param time The time we are trying to get the end of day for.
 * \returns A time representing the midnight of the next day.
 */
time_t CTimeHelper::GetEndOfDay(const time_t time)
{
	struct tm * t = gmtime(&time);
	t->tm_hour = t->tm_min = t->tm_sec = 0;
	t->tm_mday += 1;
	t->tm_yday = 0;
	return mktimeGMT(t);
}

/**
* Get's the time that is the start of the day of the given time (i.e. midnight of that day).
* \param time The time we are trying to get the start of the day for.
* \returns A time representing the midnight of the the given day.
*/
time_t CTimeHelper::GetStartOfDay(const time_t time)
{
	struct tm * t = gmtime(&time);
	t->tm_hour = t->tm_min = t->tm_sec = 0;
	return mktimeGMT(t);
}



/**
 * Add a specific amount of days to a given time/
 * \param time The time we are starting with.
 * \param days The amount of days we would like to add to that time.
 * \returns A new time with the same time of day but {days} more days into the future.
 */
time_t CTimeHelper::AddDays(const time_t * time, int days)
{
	struct tm * editedTime = gmtime(time);

	editedTime->tm_mday += days;
	editedTime->tm_yday = 0;

	return mktimeGMT(editedTime);
}


/**
 * Gets the difference in days between two times. This isn't 24 hour periods. Just days.
 * For example, the difference in days between Feb 3 and Feb 7 is 4 days, no matter what time on either
 * of those days it is.
 * \param time1 The first time. Should be the more recent one unless you're aiming for negative days.
 * \param time2 The second time.
 * \returns The amount of days between the two times. 
 */
int CTimeHelper::DiffDays(const time_t time1, const time_t time2)
{
	double diffSec = difftime(GetStartOfDay(time1), GetStartOfDay(time2));

	return ((int)diffSec) / DAYS_TO_SECS;

}

/**
* Gets the difference in days between two times. This isn't 24 hour periods. Just days.
* For example, the difference in days between Feb 3 and Feb 7 is 4 days, no matter what time on either
* of those days it is.
* \param time1 The first time struct. Should be the more recent one unless you're aiming for negative days.
* \param time2 The second time struct.
* \returns The amount of days between the two times.
*/
int CTimeHelper::DiffDays(tm * const t1, tm * const t2)
{
	t1->tm_hour = t1->tm_min = t1->tm_sec = 0;
	t2->tm_hour = t2->tm_min = t2->tm_sec = 0;

	time_t time1 = mktimeGMT(t1);
	time_t time2 = mktimeGMT(t2);
	return DiffDays(time1, time2);

	
}