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

void DateParser::calculate_absolute_time(const std::string& date)
{
	int y, m, d;
	sscanf_s(date.c_str(), "%d-%d-%d", &y, &m, &d);

	result_time = {};
	result_time.wYear = y;
	result_time.wMonth = m;
	result_time.wDay = d;
}

