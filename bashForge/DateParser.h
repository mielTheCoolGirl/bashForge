#pragma once
#include <ctime>
#include <iomanip> 
#include <regex>
#include <sstream>
#include <windows.h>
#include <string.h>

// num of 100 nanosecsintervals in one day(24*60sec*60min* 10,000,000)
#define DAY_IN_100NS_UNITS (864000000000LL)
class DateParser
{
public:
    SYSTEMTIME result_time;
    DateParser(std::string date);
    static SYSTEMTIME  parse_touch_t(const std::string& t);
private:
	
    bool parse_absolute_date(const std::string& input);

    
    bool parse_relative_date(const std::string& input);

    
    void calculate_relative_time(const std::string& time);
    void calculate_absolute_time(const std::string& date);

    
    bool is_leap_year(int year) const;
};

