#include "PlatformDateTime.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DateTime::DateTime() {
	status_ = Status::null;
	time_	= 0;
	memset(&t_, 0, sizeof(tm));
	}

DateTime::DateTime(bool initializeWithCurrentTime){
	if( initializeWithCurrentTime ){
		ASSERT(time != 0);
		time_ = time(NULL);
		status_ = Status::valid;
		memset(&t_, 0, sizeof(tm));
#ifdef _WINDOWS
		localtime_s(&t_, &time_);
#elif __APPLE__
        localtime_r(&time_, &t_);
#endif
		}
	}

DateTime::DateTime(time_t time){
	ASSERT(time != 0);
	time_ = time;
	status_ = Status::valid;
	memset(&t_, 0, sizeof(tm));
#ifdef _WINDOWS
    localtime_s(&t_, &time_);
#elif __APPLE__
    localtime_r(&time_, &t_);
#endif
	}

DateTime::DateTime(int year, int month, int day){
	SetDate(year, month, day);
	}

DateTime::DateTime(int year, int month, int day, int hour, int minute, int second){
	SetDateTime(year, month, day, hour, minute, second);
	}

DateTime::~DateTime(){
	}

DateTime& 
DateTime::operator= (time_t t){
	ASSERT(time != 0);
	time_ = t;
	status_ = Status::valid;
	memset(&t_, 0, sizeof(tm));
#ifdef _WINDOWS
    localtime_s(&t_, &time_);
#elif __APPLE__
    localtime_r(&time_, &t_);
#endif
	return (*this);
	}

bool 
DateTime::operator== (DateTime& dt){
	if( dt.GetStatus() == Status::valid && GetStatus() == Status::valid ){
		if( difftime(this->time_, dt.time_) == 0.0 )
			return true;
		else
			return false;
		}
	return false;
	}

bool
DateTime::operator> (DateTime& dt){
	if( dt.GetStatus() == Status::valid && GetStatus() == Status::valid ){
		if( difftime(this->time_, dt.time_) > 0.0 )
			return true;
		else
			return false;
		}
	return false;
	}

bool
DateTime::operator< (DateTime& dt){
	if( dt.GetStatus() == Status::valid && GetStatus() == Status::valid ){
		if( difftime(this->time_, dt.time_) < 0.0 )
			return true;
		else
			return false;
		}
	return false;
	}

int
DateTime::GetDifferenceInSeconds(DateTime& dtAfter){
	ASSERT(status_ == Status::valid && dtAfter.GetStatus() == Status::valid );
	if( status_ !=  Status::valid || dtAfter.GetStatus() != Status::valid )
		return 0;
	int seconds = (int)difftime(dtAfter, time_);
	return seconds;
	}

void
DateTime::SetCurrentTime(){
	*this = time(NULL);
	}

bool 
DateTime::SetDate(int year, int month, int day){
	tm t;
	memset(&t, 0, sizeof(tm));
	t.tm_year	= year;
	t.tm_mon	= month;
	t.tm_mday	= day;

	time_t result = mktime(&t);
	if( result != -1 ){
		time_ = result;
		status_ = Status::valid;
#ifdef _WINDOWS
		localtime_s(&t_, &time_);
#elif __APPLE__
        localtime_r(&time_, &t_);
#endif
		return true;
		}
	return false;
	}

bool
DateTime::SetDateTime(int year, int month, int day, int hour, int minute, int second){
	tm t;
	memset(&t, 0, sizeof(tm));
	t.tm_year	= year;
	t.tm_mon	= month;
	t.tm_mday	= day;
	t.tm_hour	= hour;
	t.tm_min	= minute;
	t.tm_sec	= second;

	time_t result = mktime(&t);
	if( result != -1 ){
		time_	= result;
		status_ = Status::valid;
#ifdef _WINDOWS
		localtime_s(&t_, &time_);
#elif __APPLE__
        localtime_r(&time_, &t_);
#endif
		return true;
		}
	return false;
	}

bool
DateTime::ParseDateTime(LPCSTR lpszDateTime){
	return false;
	}

bool
DateTime::ParseDate(LPCSTR lpszDate){
	return false;
	}

bool
DateTime::ParseTime(LPCSTR lpszTime){
	return false;
	}