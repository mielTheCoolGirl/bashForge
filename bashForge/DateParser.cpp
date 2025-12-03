#include "DateParser.h"

DateParser::DateParser(std::string date)
{
	result_time = {};
	if (parse_absolute_date(date))
		calculate_absolute_time(date);
	else if (parse_relative_date(date))
		calculate_relative_time(date); //TO DO, MAKE THIS HAPPEN
	
}

bool DateParser::parse_absolute_date(const std::string& input)
{
	// A date regex which makes you follow the pattern of YYYY-MM-DD
	std::regex date_regex("^(19|20)\\d{2}\\-(0[1-9]|1[0-2])\\-(0[1-9]|[12][0-9]|3[01])$");
	return std::regex_match(input, date_regex);
}

bool DateParser::parse_relative_date(const std::string& input)
{
	std::string date = input;
	static const std::vector<std::string> weekdays = { "monday","tuesday","wednesday","thursday", "friday","saturday","sunday" };
	static const std::vector<std::string> simple_phrases = { "today", "tomorrow", "yesterday","last week", "next week" };

	for (const auto& w : simple_phrases) //check for the simplest options available
	{
		if (date == w) return true;
	}
	for (const auto& d : weekdays)  //check for date setting
	{
		if (date == "next " + d) return true;
		if (date == "last " + d) return true;
	}
	static std::regex in_days(R"(in\s+([0-9]+)\s+days?)");
	static std::regex days_ago(R"(([0-9]+)\s+days?\s+ago)");
	std::smatch regex_match;

	if (std::regex_match(date, regex_match, in_days))
	{
		int n = std::stoi(regex_match[1]);
		return n >= 1 && n <= 365;  // reasonable day range
	}

	if (std::regex_match(date, regex_match, days_ago))
	{
		int n = std::stoi(regex_match[1]);
		return n >= 1 && n <= 365;
	}

	//if its doesnt fit any setting
	return false;
}

void DateParser::calculate_relative_time(const std::string& time)
{
	SYSTEMTIME current_time;
	GetLocalTime(&current_time);
	FILETIME ft = { 0 };
	ULONGLONG time_value=(((ULONGLONG)ft.dwHighDateTime) << 32) + ft.dwLowDateTime;
	long long days_offset = 0;

	if (time == "tomorrow")
		days_offset = 1;
	else if (time == "yesterday")
		days_offset = -1;
	else if (time == "next week")
		days_offset = 7;
	else if (time == "last week")
		days_offset = -7;
	
	
	static std::regex in_days(R"(in\s+([0-9]+)\s+days?)");
	static std::regex days_ago(R"(([0-9]+)\s+days?\s+ago)");
	std::smatch regex_match;
	if (std::regex_match(time, regex_match, in_days)) {
		days_offset = std::stoi(regex_match[1]);
	}
	else if (std::regex_match(time, regex_match, days_ago)) {
		days_offset = -std::stoi(regex_match[1]);
	}

	std::vector<std::string> weekdays = { "monday","tuesday","wednesday","thursday", "friday","saturday","sunday" };
	int current_dow = current_time.wDayOfWeek;
	for (int i = 0; i < weekdays.size(); i++)
	{
		int target_day_of_week; //day of the week -> sun=0, mon=1... sat=6
		if (i < 6)
			target_day_of_week = i + 1;
		else
			target_day_of_week = 0;

		// check for 'next [weekday]'
		if (time == "next " + weekdays[i])
		{
			//calc days until next target day (1 to 7 days)
			days_offset = (target_day_of_week - current_dow + 7) % 7;
			if (days_offset == 0) days_offset = 7; // If today is the target, 'next' means 7 days later
			break;
		}
		// check for 'last [weekday]'
		else if (time == "last " + weekdays[i])
		{
			//calc days since last target day (-1 to -7 days)
			days_offset = (target_day_of_week - current_dow - 7) % 7;
			if (days_offset == 0) days_offset = -7; // If today is the target, 'last' means 7 days prior
			break;
		}
	}
	time_value += days_offset * DAY_IN_100NS_UNITS;
	ft.dwLowDateTime = (DWORD)time_value;
	ft.dwHighDateTime = (DWORD)(time_value >> 32); //converting back to filetime
	FileTimeToSystemTime(&ft, &result_time);

}

void DateParser::calculate_absolute_time(const std::string& date)
{
	int y, m, d;
	sscanf_s(date.c_str(), "%d-%d-%d", &y, &m, &d);
	result_time = {};
	result_time.wYear = y;
	result_time.wMonth = m;
	result_time.wDay = d;
}

bool DateParser::is_leap_year(int year) const
{
	return (year%4==0 && year % 100 != 0) || (year % 400 == 0);
}

