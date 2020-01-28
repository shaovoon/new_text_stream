// TestTextStream.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "OverloadStreamOperators.h"
#include "NewTextStream.h"

void TestStringInput();
void TestNormal();
void TestSetPrecision();
void TestLeadingZeroes();
void TestLaggingSpaces();
void TestTrim();
void TestOverloaded();
void TestEscape();
void TestReadIni();
void TestHex();

struct Product
{
	Product() : name(""), qty(0), price(0.0f) {}
	Product(std::string name_, int qty_, float price_) : name(name_), qty(qty_), price(price_) {}
	std::string name;
	int qty;
	float price;
};


/*
new_text::ofstream& operator << (new_text::ofstream& ostm, const Date& date)
{
	new_text::ofstream ofs_temp;
	ofs_temp << date.year << date.month << date.day;
	ofs_temp.match("{0}-{1:02}-{2:02}");

	ostm.push_back(ofs_temp.str());

	return ostm;
}

new_text::ifstream& operator >> (new_text::ifstream& istm, Date& date)
{
	new_text::ifstream ifs_temp;
	ifs_temp.set_string(istm.pop());
	ifs_temp.match("{0}-{1:02}-{2:02}");
	ifs_temp >> date.year >> date.month >> date.day;

	return istm;
}
*/


// test whitespace trim
int main(int argc, char* argv[])
{
	std::cout << "TestStringInput();" << std::endl;
	TestStringInput();
	/*
	std::cout << "TestNormal();" << std::endl;
	TestNormal();
	std::cout << "TestSetPrecision();" << std::endl;
	TestSetPrecision();
	std::cout << "TestLeadingZeroes();" << std::endl;
	TestLeadingZeroes();
	std::cout << "TestLaggingSpaces();" << std::endl;
	TestLaggingSpaces();
	std::cout << "TestTrim();" << std::endl;
	TestTrim();
	std::cout << "TestOverloaded();" << std::endl;
	TestOverloaded();
	std::cout << "TestEscape();" << std::endl;
	TestEscape();
	std::cout << "TestReadIni();" << std::endl;
	TestReadIni();
	std::cout << "TestHex();" << std::endl;
	TestHex();
	*/
	return 0;
}

void TestStringInput()
{
	try
	{
		new_text::ifstream is;

		std::string log_line = "2020-01-16 18:23:56.020 NEW_USER UserName=Kelly, DoB=1999-01-02";
		log_line = log_line.substr(24); // get rid of the log timestamp
		is.str(log_line);
		/*
		if (is.match("NEW_USER UserName={0}, DoB={1}"))
		{
			std::string name = "";
			std::string date = "";
			is >> name >> date;
			std::cout << name << ":" << date << std::endl;
		}
		*/
		if (is.match("NEW_USER UserName={0}, DoB={1:t}-{2:02}-{3:02}")) // Parse date in YYYY-MM-DD
		{
			std::string name = "";
			int year = 0, month = 0, day = 0;
			is >> name >> year >> month >> day;
			std::cout << name << ":" << year << "-" << month << "-" << day << std::endl;
		}
	}
	catch (std::exception & e)
	{
		std::cout << "Exception thrown:" << e.what() << std::endl;
	}

}

void TestNormal()
{
	try
	{
		new_text::ofstream os("products.txt", std::ios_base::out);
		if(os.is_open())
		{
			Product product("Shampoo", 200, 15.0f);
			os << product.name << product.qty << product.price;
			os.match("{0},{1},{2}");
			os.write_line();
			Product product1("Soap", 300, 25.0f);
			os << product1.name << product1.qty << product1.price;
			os.match("{0},{1},{2}");
			os.write_line();
		}
		os.flush();
		os.close();

		new_text::ifstream is("products.txt", std::ios_base::in);
		if(is.is_open())
		{
			Product temp;
			while(is.read_line())
			{
				if(is.match("{0},{1},{2}"))
				{
					is >> temp.name >> temp.qty >> temp.price;
					// display the read items
					std::cout << temp.name << "," << temp.qty << "," << temp.price << std::endl;
				}
			}
		}
	}
	catch(std::exception& e)
	{
		std::cout << "Exception thrown:" << e.what() << std::endl;
	}
}

void TestSetPrecision()
{
	try
	{
		new_text::ofstream os("products.txt", std::ios_base::out);
		if(os.is_open())
		{
			os.set_precision(17);
			Product product("Shampoo", 200, 15.83f);
			os << product.name << product.qty << product.price;
			os.match("{0},{1},{2}");
			os.write_line();
			Product product1("Soap", 300, 25.45f);
			os << product1.name << product1.qty << product1.price;
			os.match("{0},{1},{2}");
			os.write_line();
		}
		os.flush();
		os.close();

		new_text::ifstream is("products.txt", std::ios_base::in);
		if(is.is_open())
		{
			Product temp;
			while(is.read_line())
			{
				if(is.match("{0},{1},{2}"))
				{
					is >> temp.name >> temp.qty >> temp.price;
					// display the read items
					std::cout << temp.name << "," << temp.qty << "," << temp.price << std::endl;
				}
			}
		}
	}
	catch(std::exception& e)
	{
		std::cout << "Exception thrown:" << e.what() << std::endl;
	}
}

void TestLeadingZeroes()
{
	try
	{
		new_text::ofstream os("products.txt", std::ios_base::out);
		if(os.is_open())
		{
			Product product("Shampoo", 200, 15.83f);
			os << product.name << product.qty << product.price;
			os.match("{0},{1:05},{2}");
			os.write_line();
		}
		os.flush();
		os.close();

		new_text::ifstream is("products.txt", std::ios_base::in);
		if(is.is_open())
		{
			Product temp;
			while(is.read_line())
			{
				if(is.match("{0},{1:05},{2}"))
				{
					is >> temp.name >> temp.qty >> temp.price;
					// display the read items
					std::cout << temp.name << "," << temp.qty << "," << temp.price << std::endl;
				}
			}
		}
	}
	catch(std::exception& e)
	{
		std::cout << "Exception thrown:" << e.what() << std::endl;
	}
}

void TestLaggingSpaces()
{
	try
	{
		new_text::ofstream os("products.txt", std::ios_base::out);
		if(os.is_open())
		{
			Product product("Shampoo", 200, 15.83f);
			os << product.name << product.qty << product.price;
			os.match("{0},{1:5},{2}");
			os.write_line();
		}
		os.flush();
		os.close();

		new_text::ifstream is("products.txt", std::ios_base::in);
		if(is.is_open())
		{
			Product temp;
			while(is.read_line())
			{
				if(is.match("{0},{1:5},{2}"))
				{
					is >> temp.name >> temp.qty >> temp.price;
					// display the read items
					std::cout << temp.name << "," << temp.qty << "," << temp.price << std::endl;
				}
			}
		}
	}
	catch(std::exception& e)
	{
		std::cout << "Exception thrown:" << e.what() << std::endl;
	}
}

void TestTrim()
{
	try
	{
		new_text::ofstream os("products.txt", std::ios_base::out);
		if(os.is_open())
		{
			os.set_precision(17);
			Product product("  \tShampoo  ", 200, 15.83f);
			os << product.name << product.qty << product.price;
			os.match("{0},{1},{2}");
			os.write_line();
		}
		os.flush();
		os.close();

		new_text::ifstream is("products.txt", std::ios_base::in);
		if(is.is_open())
		{
			Product temp;
			while(is.read_line())
			{
				if(is.match("{0:t},{1},{2}"))
				{
					is >> temp.name >> temp.qty >> temp.price;
					// display the read items
					std::cout << temp.name << "," << temp.qty << "," << temp.price << std::endl;
				}
			}
		}
	}
	catch(std::exception& e)
	{
		std::cout << "Exception thrown:" << e.what() << std::endl;
	}
}

void TestOverloaded()
{
	try
	{
		new_text::ofstream os("products.txt", std::ios_base::out);
		if(os.is_open())
		{
			os.set_precision(17);
			Product product("Shampoo", 200, 15.83f);
			Date date(2014,9,5);
			os << product.name << date << product.qty << product.price;
			os.match("{0},{1},{2},{3}");
			os.write_line();
		}
		os.flush();
		os.close();

		new_text::ifstream is("products.txt", std::ios_base::in);
		if(is.is_open())
		{
			Product prod;
			Date date1;
			while(is.read_line())
			{
				if(is.match("{0},{1},{2},{3}"))
				{
					is >> prod.name >> date1 >> prod.qty >> prod.price;
					// display the read items
					std::cout << prod.name << "," << date1 << "," << prod.qty << "," << prod.price << std::endl;

					//std::cout << prod.name << "," << date1.year << "-" << date1.month 
					//	<< "-" << date1.day << "," << prod.qty << "," << prod.price << std::endl;
				}
			}
		}
	}
	catch(std::exception& e)
	{
		std::cout << "Exception thrown:" << e.what() << std::endl;
	}

}

void TestEscape()
{
	try
	{
		new_text::ofstream os("products.txt", std::ios_base::out);
		if(os.is_open())
		{
			os.set_precision(17);
			Product product("Shampoo", 200, 15.83f);
			Date date(2014,9,5);
			os << product.name << date << product.qty << product.price;
			os.match("\\{{0}\\},{1},{2},{3}");
			os.write_line();
		}
		os.flush();
		os.close();

		new_text::ifstream is("products.txt", std::ios_base::in);
		if(is.is_open())
		{
			Product prod;
			Date date1;
			while(is.read_line())
			{
				if(is.match("\\{{0}\\},{1},{2},{3}"))
				{
					is >> prod.name >> date1 >> prod.qty >> prod.price;
					// display the read items
					std::cout << prod.name << "," << date1 << "," << prod.qty << "," << prod.price << std::endl;

					//std::cout << prod.name << "," << date1.year << "-" << date1.month 
					//	<< "-" << date1.day << "," << prod.qty << "," << prod.price << std::endl;
				}
			}
		}
	}
	catch(std::exception& e)
	{
		std::cout << "Exception thrown:" << e.what() << std::endl;
	}

}

void TestReadIni()
{
	try
	{
		new_text::ifstream is("settings.ini", std::ios_base::in);
		if(is.is_open())
		{
			while(is.read_line())
			{
				if(is.match("#{0:t}", false)) // Parse comment
				{
					is.match("#{0:t}");
					std::string comment="";
					is >> comment;
					std::cout << "Comment:" << comment << std::endl;
				}
				else if(is.match("[{0}]", false)) // Parse section
				{
					is.match("[{0}]");
					std::string section="";
					is >> section;
					std::cout << "Section:" << section << std::endl;
				}
				else if(is.match("{0:t}={1:t}", false)) // Parse name/value
				{
					if(is.match("{0:t}={1:t},{2:t},{3:t}", false)) // Parse RGB
					{
						std::cout<<"Parse RGB"<<std::endl;
						is.match("{0:t}={1:t},{2:t},{3:t}");
						std::string name="";
						int red=0, green=0, blue=0;
						is >> name >> red >> green >> blue;
						std::cout << name << ":" << "r:" << red << ", g:" 
						    << green << ", b:" << blue << std::endl;
					}
					else if(is.match("{0:t}={1:t}-{2:02}-{3:02}", false)) // Parse date in YYYY-MM-DD
					{
						is.match("{0:t}={1:t}-{2:02}-{3:02}");
						std::string name="";
						int year=0, month=0, day=0;
						is >> name >> year >> month >> day;
						std::cout << name << ":" << year << "-" << month << "-" << day << std::endl;
					}
					else // Parse normal name/value
					{
						is.match("{0:t}={1:t}");
						std::string name="", value="";
						is >> name >> value;
						std::cout << name << ":" << value << std::endl;
					}
				}
			}
		}
	}
	catch(std::exception& e)
	{
		std::cout << "Exception thrown:" << e.what() << std::endl;
	}
}

void TestHex()
{
	try
	{
		new_text::ofstream os("products.txt", std::ios_base::out);
		if(os.is_open())
		{
			os.set_precision(17);
			Product product("  \tShampoo  ", 1234, 15.83f);
			os << product.name << product.qty << product.price;
			os.match("{0},{1:x},{2}");
			os.write_line();
		}
		os.flush();
		os.close();

		new_text::ifstream is("products.txt", std::ios_base::in);
		if(is.is_open())
		{
			Product temp;
			while(is.read_line())
			{
				if(is.match("{0:t},{1:x},{2}"))
				{
					is >> temp.name >> temp.qty >> temp.price;
					// display the read items
					std::cout << temp.name << "," << temp.qty << "," << temp.price << std::endl;
				}
			}
		}
	}
	catch(std::exception& e)
	{
		std::cout << "Exception thrown:" << e.what() << std::endl;
	}
}
