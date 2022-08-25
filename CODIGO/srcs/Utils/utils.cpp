#include "utils.hpp"

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
