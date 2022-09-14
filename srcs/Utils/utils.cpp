#include "utils.hpp"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

float time_diff2(struct timespec *start, struct timespec *end){
    return (end->tv_sec - start->tv_sec) + 1e-9*(end->tv_nsec - start->tv_nsec);
}
// Up to capitalize
void capitalizeString(const std::string& in, std::string& out)
{
    std::string::const_iterator b = in.begin(), e = in.end(); 
    // skipping leading spaces 
    std::string tmp;
    while (b != e)
    {
        //std::cout << (*b) << " " << (char)std::toupper(*b) << std::endl;
        tmp.push_back(std::tolower(*b));
        //std::cout << tmp << std::endl;
        b++;
    } 
    //tmp.push_back(0);
    std::string::const_iterator init = tmp.begin(), end = tmp.end(); 
    out.assign(init, end); 
}

//std::string lowerString()

// Check if c is a space
bool isSpace(char c)
{ return (c == ' ');}

bool isTab(char c)
{    return (c == '\t');}

void stripTab(const std::string& in, std::string& out) 
{ 
    std::string::const_iterator b = in.begin(), e = in.end(); 
    // skipping leading spaces 
    while (isTab(*b)){ ++b; } if (b != e)
    { 
        // skipping trailing spaces 
        while (isTab(*(e-1)))
        { --e; } 
    } 
    out.assign(b, e); 
}

// Delete spaces 
void strip(const std::string& in, std::string& out) 
{ 
    std::string::const_iterator b = in.begin(), e = in.end(); 
    // skipping leading spaces 
    while (isSpace(*b)){ ++b; } if (b != e)
    { 
        // skipping trailing spaces 
        while (isSpace(*(e-1)))
        { --e; } 
    } 
    out.assign(b, e); 
}

std::string ft_clean_token(const std::string& in)
{
    std::string::const_iterator b = in.begin(), e = in.end(); 
    // skipping leading spaces 
    std::string out;

    while (b != e)
    {
        //std::cout << (*b) << " " << (char)std::toupper(*b) << std::endl;
        if (!isSpace(*(b)) && !isTab(*(b)) )
            out.push_back(std::tolower(*b));
        //std::cout << tmp << std::endl;
        b++;
    } 
    return out;
}

// Give the number of tokens split with delimiter string
int nb_tokens(std::string str, char delimiter)
{
    std::istringstream iss(str);
    std::string token;
    int ret=0;
    while (std::getline(iss, token, delimiter))
    {
        ret++;
    }
    return ret;
}

// Give the token in nb position split with delimiter string
std::string  get_token(std::string str, char delimiter, int nb)
{
    std::istringstream iss(str);
    std::string token;
    int ret=0;
    while (std::getline(iss, token, delimiter))
    {
        //std::cout << token << std::endl;
        if (ret == nb)
            break;
        ret++;
    }
    return token;
}

// True if file exits false is not
bool file_exits(const std::string name)
{
    int fd;
    fd = open(name.c_str(), O_RDONLY);
    if (fd < 0){
        return (false);
    }
    else {
        return(true);
    }
}
