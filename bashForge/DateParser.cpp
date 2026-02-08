#include "DateParser.h"

DateParser::DateParser(std::string date)
{
	result_time = {};
	if (parse_absolute_date(date))
		calculate_absolute_time(date);
	else if (parse_relative_date(date))
		calculate_relative_time(date); 
	
}

bool DateParser::parse_absolute_date(const std::string& input)
{
	// A date regex which makes you follow the pattern of YYYY-MM-DD or YYYY-MM-DD HH:MM
	std::regex date_regex(R"(^(19|20)\d{2}-(0[1-9]|1[0-2])-(0[1-9]|[12][0-9]|3[01])( ([01][0-9]|2[0-9]):([0-5][0-9]))?$)");
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
	SYSTEMTIME current_sys_time;
	GetLocalTime(&current_sys_time);

	FILETIME current_ft;
	SystemTimeToFileTime(&current_sys_time, &current_ft);
	ULONGLONG time_value = ((ULONGLONG)current_ft.dwHighDateTime << 32) | current_ft.dwLowDateTime;
	long long days_offset = 0;

	if (time == "tomorrow")
		days_offset = 1;
	else if (time == "yesterday")
		days_offset = -1;
	else if (time == "next week")
		days_offset = 7;
	else if (time == "last week")
		days_offset = -7;

	else
	{
		static std::regex in_days(R"(in\s+([0-9]+)\s+days?)");
		static std::regex days_ago(R"(([0-9]+)\s+days?\s+ago)");
		std::smatch regex_match;

		if (std::regex_match(time, regex_match, in_days)) {
			days_offset = std::stoi(regex_match[1]);
		}
		else if (std::regex_match(time, regex_match, days_ago)) {
			days_offset = -std::stoi(regex_match[1]);
		}

		// --- Phase 3: Next/Last Weekday ---
		else
		{
			// Use an array that aligns with wDayOfWeek: 0=Sun, 1=Mon, ..., 6=Sat
			std::string weekdays[] = { "sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday" };
			int current_dow = current_sys_time.wDayOfWeek; // 0-6

			for (int target_dow = 0; target_dow < 7; target_dow++)
			{
				// Check for 'next [weekday]'
				if (time == "next " + weekdays[target_dow])
				{
					// Calculate positive difference (1 to 7 days)
					days_offset = (target_dow - current_dow + 7) % 7;
					if (days_offset == 0) days_offset = 7; // 'next' means 7 days later if today is the target
					break;
				}
				// Check for 'last [weekday]'
				else if (time == "last " + weekdays[target_dow])
				{
					// Calculate negative difference (-1 to -7 days)
					days_offset = (target_dow - current_dow - 7) % 7;
					if (days_offset == 0) days_offset = -7; // 'last' means 7 days prior if today is the target
					break;
				}
			}
		}
	}
	if (days_offset != 0 ||
		time == "tomorrow" || time == "yesterday" || time == "next week" || time == "last week" ||
		std::regex_match(time, std::regex(R"([0-9]+ (days?|weeks?) (ago|later))")) // simplified regex check for full coverage
		)
	{
		// Apply the calculated day offset
		time_value += days_offset * DAY_IN_100NS_UNITS;

		FILETIME new_ft;
		new_ft.dwLowDateTime = (DWORD)time_value;
		new_ft.dwHighDateTime = (DWORD)(time_value >> 32);

		// Convert the new FILETIME to SYSTEMTIME for result_time
		FileTimeToSystemTime(&new_ft, &result_time);
	}
}

void DateParser::calculate_absolute_time(const std::string& date)
{
	int y = 0, m = 0, d = 0, hh = 0, mm = 0;
	if (sscanf_s(date.c_str(), "%d-%d-%d %d:%d", &y, &m, &d, &hh, &mm) >= 3)
	{
		result_time = {};
		result_time.wYear = y;
		result_time.wMonth = m;
		result_time.wDay = d;
		result_time.wHour = hh;
		result_time.wMinute = mm;
	}
}

SYSTEMTIME DateParser::parse_touch_t(const std::string& t)
{
	SYSTEMTIME st = {};
	int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;

	std::string mainPart = t;
	if (t.find('.') != std::string::npos) //find if there are seconds in the time
	{
		mainPart = t.substr(0, t.find('.'));
		second = std::stoi(t.substr(t.find('.') + 1));
	}
	if (mainPart.length() == 12)
	{
		sscanf_s(mainPart.c_str(), "%4d%2d%2d%2d%2d", &year, &month, &day, &hour, &minute); //put CCYYMMDDhhmm in a SYSTEMTIME pattern
	}
	else if (mainPart.length() == 10) 
	{
		int yy;
		sscanf_s(mainPart.c_str(), "%2d%2d%2d%2d%2d", &yy, &month, &day, &hour, &minute);
		year = yy + 2000; //making an assumption that we're starting from the year 2000
	}
	else 
	{
		throw std::runtime_error("Invalid -t timestamp format");
	}
	st.wYear = year;
	st.wMonth = month;
	st.wDay = day;
	st.wHour = hour;
	st.wMinute = minute;
	st.wSecond = second;

	return st;
}

bool DateParser::is_leap_year(int year) const
{
	return (year%4==0 && year % 100 != 0) || (year % 400 == 0);
}

