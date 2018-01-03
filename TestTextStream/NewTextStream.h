// The MIT License (MIT)
// New Text Streams 0.7.0
// Copyright (C) 2014, by Wong Shao Voon (shaovoon@yahoo.com)
//
// http://opensource.org/licenses/MIT
//

//#define USE_BOOST_LEXICAL_CAST
//#define USE_BOOST_TRIM

#ifndef NEWTEXTSTREAM_H
#define NEWTEXTSTREAM_H

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <cstring>
#include <stdexcept>

#ifdef USE_BOOST_LEXICAL_CAST
#	include <boost/lexical_cast.hpp>
#endif

#ifdef USE_BOOST_TRIM
#	include <boost/algorithm/string.hpp>
#else
#	include <cctype>
// trim from left
std::string &ltrim(std::string &s);
// trim from right
std::string &rtrim(std::string &s);
// trim from both ends
std::string &trim(std::string &s);
// trim from left of all zeroes
std::string &ltrim_zero(std::string &s);
#endif

std::string replace_all(std::string subject, const std::string& search, const std::string& replace);
bool set_hex(std::string& dest, unsigned int val, bool bAddPrefix = false);
bool check_hex_string(const std::string& str);
bool read_hex(const std::string& src, unsigned int& val);


namespace new_text
{
	class ifstream
	{
	public:
		ifstream() 
		{
			init();
		}
		ifstream(const char * file, std::ios_base::openmode mode)
		{
			open(file, mode);
		}
		void open(const char * file, std::ios_base::openmode mode)
		{
			init();
			istm.open(file, mode);
		}
		void init()
		{
			line = "";
		}
		void close()
		{
			istm.close();
		}
		bool is_open()
		{
			return istm.is_open();
		}
		bool eof() const
		{
			return (istm.eof()&&line == "");
		}
		bool match(const std::string& format, bool process = true)
		{
			if(line.empty()||format.empty())
				return false;

			// clear queue
			while(que.size()>0)
			{
				que.pop();
			}

			size_t line_pos = 0;
			size_t curr_index = -1;
			std::string curr_str = "";
			std::vector<size_t> vec_index; // for error checking
			bool to_be_trimmed = false;
			bool zeros_to_be_trimmed = false;
			bool conv_from_hex = false;
			size_t max_chars = 0;
			size_t max_chars_start = 0;
			size_t i = 0;
			for(; i<format.size(); ++i)
			{
				if(format.at(i)=='\\')
				{
					if(i+1<format.size()&&(format.at(i+1)=='{'||format.at(i+1)=='}'))
					{
						if(!push_string(format, i, line_pos, curr_str, max_chars, max_chars_start, process, 
							to_be_trimmed, zeros_to_be_trimmed, conv_from_hex))
							return false;
						if(line_pos>=line.size())
							break;
					}
				}
				else if(i<format.size()&&format.at(i)=='{')
				{
					if(!curr_str.empty())
					{
						if(process)
						{
							further_processing( to_be_trimmed, curr_str, zeros_to_be_trimmed, conv_from_hex );
							que.push(curr_str);
						}
						curr_str = "";
					}
					to_be_trimmed = false;
					zeros_to_be_trimmed = false;
					conv_from_hex = false;

					size_t pos1 = format.find(':', i);
					size_t pos2 = format.find('}', i);
					if(pos2<pos1) // '}' found
					{
						std::string num_str = format.substr(i+1, pos2-(i+1));
						std::istringstream stm;
						stm.str(num_str);
						stm >> curr_index;
						vec_index.push_back(curr_index);
						i = pos2 + 1;

						if(!push_string(format, i, line_pos, curr_str, max_chars, max_chars_start, process, 
							to_be_trimmed, zeros_to_be_trimmed, conv_from_hex))
							return false;
						if(line_pos>=line.size())
							break;
					}
					else if(pos1<pos2) // ':' found
					{
						std::string num_str = format.substr(i+1, pos1-(i+1));
						std::istringstream stm;
						stm.str(num_str);
						stm >> curr_index;
						vec_index.push_back(curr_index);

						size_t pos3 = format.find('}', i);
						if(pos3!=std::string::npos)
						{
							i = pos3+1;
							std::string fmt_specifier_str = format.substr(pos1+1, pos3-(pos1+1));

							if(!fmt_specifier_str.empty()&&fmt_specifier_str.at(0)=='0')
							{
								fmt_specifier_str = fmt_specifier_str.substr(1, fmt_specifier_str.size()-1);
								zeros_to_be_trimmed = true;
							}
							if(fmt_specifier_str.find('t')!=std::string::npos)
							{
								to_be_trimmed = true;
							}
							else if(fmt_specifier_str.find('x')!=std::string::npos)
							{
								conv_from_hex = true;
							}
							else
							{
								std::stringstream stm;
								stm.str(fmt_specifier_str);
								stm >> max_chars;
								max_chars_start = line_pos;
							}

							if(!push_string(format, i, line_pos, curr_str, max_chars, max_chars_start, process, 
								to_be_trimmed, zeros_to_be_trimmed, conv_from_hex))
								return false;
							if(line_pos>=line.size())
								break;

						}
					}
				}
				else
				{
					if(!push_string(format, i, line_pos, curr_str, max_chars, max_chars_start, process, 
						to_be_trimmed, zeros_to_be_trimmed, conv_from_hex))
						return false;
					if(line_pos>=line.size())
						break;
				}
			}
			if(!push_string(format, i, line_pos, curr_str, max_chars, max_chars_start, process, 
				to_be_trimmed, zeros_to_be_trimmed, conv_from_hex))
				return false;

			// check if the placeholders are consecutive
			std::sort(vec_index.begin(), vec_index.end());
			for(size_t i=0; i<vec_index.size(); ++i)
			{
				if(vec_index[i]!=i)
					throw std::out_of_range("The format placeholder index are not consecutive!");
			}

			return true;
		}
private:
		bool push_string( const std::string &format, size_t& i, size_t &line_pos, std::string &curr_str, 
			size_t max_chars, size_t max_chars_start, bool process, bool& to_be_trimmed, bool& zeros_to_be_trimmed,
			bool& conv_from_hex)
		{
			if(i<format.size())
			{
				char ch = format.at(i);
				size_t j = i+1;
				if(ch=='\\'&&j<format.size()&&(format.at(j)=='{'||format.at(j)=='}'))
				{
					i += 1;
					ch = format.at(i);
				}

				while(line_pos<line.size()&&line.at(line_pos)!=ch)
				{
					curr_str += line.at(line_pos);
					if(max_chars>0&&(max_chars_start+max_chars-1)==line_pos)
					{
						if(process)
						{
							further_processing( to_be_trimmed, curr_str, zeros_to_be_trimmed, conv_from_hex );
							que.push(curr_str);
							++line_pos;
						}
						curr_str = "";
						max_chars = max_chars_start = 0;
						break;
					}
					++line_pos;
				}
				if(line_pos>=line.size()&&line.at(line_pos-1)!=ch)
				{
					++line_pos;
					return false;
				}
				++line_pos;
			}
			else
			{
				if(line_pos<(line.size()-1))
				{
					curr_str = line.substr(line_pos, line.size()-line_pos); // extract end of line.
					line_pos = line.size();
				}
			}
			if(!curr_str.empty())
			{
				if(process)
				{
					further_processing( to_be_trimmed, curr_str, zeros_to_be_trimmed, conv_from_hex );
					que.push(curr_str);
				}
				curr_str = "";
			}
			return true;
		}

		void further_processing( bool &to_be_trimmed, std::string & curr_str, bool &zeros_to_be_trimmed, bool &conv_from_hex )
		{
#ifdef USE_BOOST_TRIM
			if(to_be_trimmed)
			{
				boost::algorithm::trim(curr_str);
				to_be_trimmed = false;
			}
			if(zeros_to_be_trimmed)
			{
				boost::algorithm::trim_left_if(curr_str, boost::algorithm::is_any_of("0"));
				zeros_to_be_trimmed = false;
			}
#else
			if(to_be_trimmed)
			{
				trim(curr_str);
				to_be_trimmed = false;
			}
			if(zeros_to_be_trimmed)
			{
				ltrim_zero(curr_str);
				zeros_to_be_trimmed = false;
			}
#endif
			if(conv_from_hex)
			{
				unsigned int val = 0;
				if(!read_hex(curr_str, val))
				{
					std::ostringstream stm;
					stm << "Invalid hex string: " << curr_str;
					throw std::runtime_error(stm.str());
				}
				else
				{
					std::ostringstream stm;
					stm << val;
					curr_str = stm.str();
				}
				conv_from_hex = false;
			}
		}

public:
		std::string pop()
		{
			std::string dest = que.front();
			que.pop();
			return dest;
		}
		bool read_line()
		{
			if(!istm.eof())
			{
				std::getline(istm, line); 
				return true;
			}
			return false;
		}
		void str(const std::string& str)
		{
			line = str;
		}
	private:
		std::ifstream istm;
		std::string line;
		std::queue<std::string> que;
	};

	class ofstream
	{
	public:

		ofstream()
		{
			init();
		}
		ofstream(const char * file, std::ios_base::openmode mode)
		{
			open(file, mode);
		}
		void open(const char * file, std::ios_base::openmode mode)
		{
			init();
			ostm.open(file, mode);
		}
		void init()
		{
			line = "";
			precision = 0;
		}
		void flush()
		{
			ostm.flush();
		}
		void close()
		{
			ostm.close();
		}
		bool is_open()
		{
			return ostm.is_open();
		}
		std::ofstream& get_internal_ofstream()
		{
			return ostm;
		}
		void set_precision(int n)
		{
			precision = n;
		}
		int get_precision()
		{
			return precision;
		}
		void push_back(const std::string& val)
		{
			vec.push_back(val);
		}
		bool match(const std::string& format)
		{
			if(vec.size()<=0||format.empty())
				return false;

			line = replace( format );
			vec.clear();
			return true;
		}
		void write_line()
		{
			ostm << line << std::endl;
		}
		std::string str()
		{
			return line;
		}
	private:
		std::string replace( const std::string& format )
		{
			std::string formatted = format;
			bool escape = false;
			if(formatted.find("\\{") != std::string::npos || formatted.find("\\}") != std::string::npos)
				escape = true;

			for( size_t i=0; i<vec.size(); ++i )
			{
				std::string whole_anchor = "";
				size_t pos = 0;
				size_t leading_zeros=0;
				size_t lagging_spaces=0;
				bool conv_to_hex = false;
				pos = find(formatted, i, pos, whole_anchor, leading_zeros, lagging_spaces, conv_to_hex);

				if( std::string::npos != pos )
				{
					formatted.erase( pos, whole_anchor.size() );
					std::string src = further_processing(vec.at(i), 
						leading_zeros, lagging_spaces, conv_to_hex);
					formatted.insert( pos, src );
					pos += src.size();
				}
				else
				{
					std::ostringstream stm;
					stm << "The format placeholder cannot be found, index: ";
					stm << i;
					throw std::out_of_range(stm.str());
				}
			}
			if(escape)
			{
				formatted = replace_all(formatted, "\\{", "{");
				formatted = replace_all(formatted, "\\}", "}");
			}

			return formatted;
		}

		std::string further_processing(const std::string& str, 
			size_t leading_zeros, size_t lagging_spaces, bool conv_to_hex)
		{
			std::string dest = str;
			if(conv_to_hex)
			{
				unsigned int val = 0;
				std::istringstream stm;
				stm.str(str);
				stm >> val;
				set_hex(dest, val);
			}
			else if(leading_zeros>0&&str.size()<leading_zeros)
			{
				std::string leading_zeros_str(leading_zeros-str.size(), '0');
				dest = leading_zeros_str + str;
			}
			else if(lagging_spaces>0&&str.size()<lagging_spaces)
			{
				std::string lagging_spaces_str(lagging_spaces-str.size(), ' ');
				dest = str + lagging_spaces_str;
			}

			return dest;
		}

		size_t find(const std::string& format, size_t index, size_t pos, std::string& whole_anchor, 
			size_t& leading_zeros, size_t& lagging_spaces, bool& conv_to_hex_bool)
		{
			if(format.empty())
				return std::string::npos;

			leading_zeros = 0;
			lagging_spaces = 0;
			bool found = false;
			size_t end_pos = 0;
			std::string anchor_str = anchor( index );
			while(!found)
			{
				pos = format.find( anchor_str, pos );
				if(pos!=std::string::npos)
				{
					if(pos>0&&format.at(pos-1)=='\\')
					{
						++pos;
						continue;
					}

					found = true;
					size_t format_pos = pos+anchor_str.size();
					if(format_pos<format.size()&&format.at(format_pos)==':') // format specifier
					{
						bool lagging_spaces_bool = true;
						if(format.at(++format_pos)=='0')
						{
							lagging_spaces_bool = false;
							++format_pos;
						}
						conv_to_hex_bool = false;
						if(format.at(format_pos)=='x')
						{
							conv_to_hex_bool = true;
							++format_pos;
						}

						std::string num_str="";
						while(format_pos<format.size()&&format.at(format_pos)>='0'&&format.at(format_pos)<='9')
						{
							num_str+=format.at(format_pos);
							++format_pos;
						}

						set_end_pos(format_pos, format, end_pos);

						whole_anchor.assign(format.c_str()+pos, end_pos-pos);

						if(!num_str.empty())
						{
							std::istringstream stm;
							stm.str(num_str);
							if(lagging_spaces_bool)
							{
								stm >> lagging_spaces;
							}
							else
							{
								stm >> leading_zeros;
							}
						}
					}
					else // no format specifier
					{
						set_end_pos(format_pos, format, end_pos);

						whole_anchor.assign(format.c_str()+pos, end_pos-pos);
					}
				}
				else
				{
					pos = std::string::npos;
					break;
				}
			}
			return pos;
		}

		void set_end_pos( size_t &format_pos, const std::string &format, size_t &end_pos )
		{
			if(++format_pos<format.size())
			{
				end_pos = format_pos;
			}
			else if(format.size()>0)
			{
				end_pos = format.size();
			}
		}

		std::string anchor(int i)
		{
			std::ostringstream stm;
			stm << i;

			std::string str = "{";
			str += stm.str();

			return str;
		}

		std::ofstream ostm;
		std::string line;
		std::vector<std::string> vec;
		int precision;
	};


} // ns new_text

template<typename T>
new_text::ifstream& operator >> (new_text::ifstream& istm, T& val)
{
#ifdef USE_BOOST_LEXICAL_CAST
	val = boost::lexical_cast<T>(istm.pop());
#else
	std::istringstream is(istm.pop());
	is >> val;
#endif

	return istm;
}
template<>
new_text::ifstream& operator >> (new_text::ifstream& istm, std::string& val)
{
	val = istm.pop();

	return istm;
}

template<typename T>
new_text::ofstream& operator << (new_text::ofstream& ostm, const T& val)
{
#ifdef USE_BOOST_LEXICAL_CAST
	std::string str = boost::lexical_cast<std::string>(val);
#else
	std::ostringstream stm;
	stm << val;
	std::string str = stm.str();
#endif

	ostm.push_back(str);

	return ostm;
}


template<>
new_text::ofstream& operator << (new_text::ofstream& ostm, const float& val)
{
#ifdef USE_BOOST_LEXICAL_CAST
	std::string str = boost::lexical_cast<std::string>(val);
#else
	std::ostringstream stm;
	if(ostm.get_precision()>0)
	{
		stm.precision(ostm.get_precision());
	}
	stm << val;
	std::string str = stm.str();
#endif

	ostm.push_back(str);

	return ostm;
}

template<>
new_text::ofstream& operator << (new_text::ofstream& ostm, const double& val)
{
#ifdef USE_BOOST_LEXICAL_CAST
	std::string str = boost::lexical_cast<std::string>(val);
#else
	std::ostringstream stm;
	if(ostm.get_precision()>0)
	{
		stm.precision(ostm.get_precision());
	}
	stm << val;
	std::string str = stm.str();
#endif

	ostm.push_back(str);

	return ostm;
}

template<typename T>
new_text::ofstream& operator << (new_text::ofstream& ostm, const std::string& val)
{
	ostm.push_back(val);

	return ostm;
}

#ifndef USE_BOOST_TRIM
// these trim functions are from stackoverflow
// http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from left
std::string &ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from right
std::string &rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
std::string &trim(std::string &s) {
	return ltrim(rtrim(s));
}

struct not_equal_to_zero : std::unary_function<int, bool>
{
	bool operator()(int i) const { return i != '0'; }
};

// trim from left
std::string &ltrim_zero(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_equal_to_zero()));
	return s;
}
#endif

// from stackoverflow
// http://stackoverflow.com/questions/5343190/how-do-i-replace-all-instances-of-of-a-string-with-another-string
std::string replace_all(std::string subject, const std::string& search, const std::string& replace) 
{
	  size_t pos = 0;
	  while ((pos = subject.find(search, pos)) != std::string::npos) {
		  subject.replace(pos, search.length(), replace);
		  pos += replace.length();
	  }
	  return subject;
}

bool set_hex(std::string& dest, unsigned int val, bool bAddPrefix)
{
	const int size = 30;
	char buf[size];
	std::memset( buf, 0, sizeof(buf) );
	//errno_t nRet = _ultow_s( val, buf, size, 16 );
	//int nRet = swprintf_s(buf, L"%X", val);
	sprintf(buf, "%X", val);

	if(bAddPrefix)
	{
		dest = "0x";
		dest += buf;
	}
	else
		dest = buf;

	return true;
}

bool check_hex_string(const std::string& str)
{
	for(size_t i=0; i<str.size(); ++i)
	{
		char ch = str.at(i);
		if(ch=='A'||ch=='B'||ch=='C'||ch=='D'||ch=='E'||ch=='F')
			continue;
		if(ch=='a'||ch=='b'||ch=='c'||ch=='d'||ch=='e'||ch=='f')
			continue;
		if(ch>='0'&&ch<='9')
			continue;
		if(i==1 && (ch=='x' || ch=='X') )
			continue;

		return false;
	}

	return true;
}

bool read_hex(const std::string& src, unsigned int& val)
{
	if(src.size()<=0||check_hex_string(src)==false)
	{
		return false;
	}

	std::string src2 = src;
	if (src.size() > 1 && src.at(0) == '0' && (src.at(1) == 'X' || src.at(1) == 'x'))
	{
		std::string str2 = "";
		for (size_t i = 2; i < src.size(); ++i)
			str2 += src[i];

		src2 = str2;
	}

	// Convert to lower case
	std::string src3 = "";
	for (size_t i = 0; i < src2.size(); ++i)
	{
		if(src2.at(i)=='A')
			src3 += 'a';
		else if(src2.at(i)=='B')
			src3 += 'b';
		else if(src2.at(i)=='C')
			src3 += 'c';
		else if(src2.at(i)=='D')
			src3 += 'd';
		else if(src2.at(i)=='E')
			src3 += 'e';
		else if(src2.at(i)=='F')
			src3 += 'f';
		else
			src3 += src2.at(i);
	}

	std::stringstream ss;
	ss << std::setbase(16) << src3;
	ss >> val;

	return true;
}

#endif // NEWTEXTSTREAM_H