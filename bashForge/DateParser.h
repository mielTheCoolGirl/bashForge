#pragma once
#include <ctime>
#include <iomanip> 
#include <regex>
#include <sstream>
#include <minwinbase.h>
#include <string.h>
class DateParser
{
public:
    SYSTEMTIME result_time;
    DateParser(std::string date);
private:
	
    bool parse_absolute_date(const std::string& input);

    
    bool parse_relative_date(const std::string& input);

    
    void calculate_relative_time(const std::string& time);
    void calculate_absolute_time(const std::string& date);

    
    bool is_leap_year(int year) const;
};

