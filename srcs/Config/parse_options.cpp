#include "parse_options.hpp"

extern Logger log;

Parse_options::Parse_options(int argc, char **argv): _argc(argc), _argv(argv), _path("./config/default.conf"),_level(0),_help(false),_test(false)
{}

Parse_options::~Parse_options(){}

bool Parse_options::help(void)
{return _help;}

bool Parse_options::test(void)
{return true;}

bool Parse_options::location(void)
{return true;}

std::string Parse_options::helpText(void)
{
    std::string text = "";
    text += "Usage: webserv [options] [config_file]\n";
    text += "\nOptions: \n";
    text += "\n  -h --help          : this help text\n";
    text += "\n  -t, --test         : test config and exit\n";
    text += "\n  -u, --uri          : keep location uri on routing\n";
    return (text);
}

// return false is ok, true is not ok
bool Parse_options::parse(void)
{
    int c = 1;
    int nb_config_files = 0;
    int nb_logs = 0;
    bool is_valid = false;
    while (c < _argc)
    {
        std::string tmp(_argv[c]);
        is_valid = false;               
            //it is parameter
            if ((tmp == "-h" or tmp == "--help") && is_valid == false)
            {   std::cout << "help" << std::endl; is_valid = true; _help=true;}
            if ((tmp == "-u" or tmp == "--uri") && is_valid == false)
            {    std::cout << "uri" << std::endl;is_valid = true;}
            if ((tmp == "-t" or tmp == "--test") && is_valid == false)
            {    _test = true; is_valid = true;}
            //check is a file
            if (tmp[0]!='-')
            {
                if (file_exits(tmp) && is_valid == false)
                {
                    log.print(INFO," ["+tmp+"] IS A FILE",GREEN,true);
                    std::cout << "is a file" << file_exits(tmp) << tmp << std::endl;
                    _path = tmp;
                    is_valid = true;
                    nb_config_files ++;
                }
                else 
                {
                    log.print(INFO," ["+tmp+"] IS NOT A FILE",RED,true);
                    return (true);
                }
            }
        c++;
        if (nb_config_files > 1)
        {
            log.print(INFO," TOO MANY CONFIG FILES",RED,true);
            return (true);
        }
        if (is_valid==false)
            return (true);
    }
    //check if file config define
    if(_path.empty())
        _path = "./config/default.conf";
    if (!file_exits(_path))
        return (true);
    return (false);
}
