
#ifndef OVERLOADED_STREAM_OPERATORS_H
#define OVERLOADED_STREAM_OPERATORS_H

#include <sstream>
#include <fstream>
#include <iostream>
#include "NewTextStream.h"

struct Date
{
	Date() : year(0), month(0), day(0) {}
	Date(int year_, short month_, short day_) : year(year_), month(month_), day(day_) {}
	int year;
	short month;
	short day;
};

std::ostream& operator << (std::ostream& ostm, const Date& date)
{
	new_text::ofstream ofs_temp;
	ofs_temp << date.year << date.month << date.day;
	ofs_temp.match("{0}-{1:02}-{2:02}");

	ostm << ofs_temp.str();

	return ostm;
}

std::istream& operator >> (std::istream& istm, Date& date)
{
	new_text::ifstream ifs_temp;
	std::string str;
	istm >> str;
	ifs_temp.str(str);
	ifs_temp.match("{0}-{1:02}-{2:02}");
	ifs_temp >> date.year >> date.month >> date.day;

	return istm;
}

#endif // OVERLOADED_STREAM_OPERATORS_H
