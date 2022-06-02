#include "parse_options.hpp"

Parse_options::Parse_options(int argc, char **argv): _argc(argc), _argv(argv), _path("./config/default.conf"),_level(0),_help(false)
{}

Parse_options::~Parse_options(){};

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
    text += "\n  -l, --log [LEVEL]  : set log level (between 0 and 2) \n";
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
        is_valid = false;               //it is parameter
            if ((tmp == "-h" or tmp == "--help") && is_valid == false)
            {   std::cout << "help" << std::endl; is_valid = true; _help=true;}
            if ((tmp == "-u" or tmp == "--uri") && is_valid == false)
            {    std::cout << "uri" << std::endl;is_valid = true;}
            if ((tmp == "-t" or tmp == "--test") && is_valid == false)
            {    std::cout << "test" << std::endl;is_valid = true;}
            if ((tmp == "-l" or tmp == "--log") && is_valid == false)
            {    
                c++;
                std::string aux(_argv[c]);
                int nb;
                try 
                {
                    nb = std::stoi(aux);
                }
                catch ( const std::invalid_argument &e)
                {
                    throw WebServer_Exception("level log must be a integer between 0 to 2 ");; //definirlo
                    return (true);
                }
                if (nb < 0 || nb > 2)
                {
                    throw WebServer_Exception("Log must be 0, 1 or 2");
                    return (true);
                }
                nb_logs ++;
                _level = nb;
                is_valid = true;
            }
            //check is a file
            if (file_exits(tmp) && is_valid == false)
            {
                std::cout << "is a file" << file_exits(tmp) << tmp << std::endl;
                _path = tmp;
                is_valid = true;
                nb_config_files ++;
            }
        c++;
        if (nb_config_files>1)
        {
            throw WebServer_Exception("error too many config files");
            return (true);
        }
        if (nb_logs > 1)
        {
            throw WebServer_Exception("error too log parameters");
            return (true);
        }
        if (is_valid==false)
        {
                
            throw WebServer_Exception("config file not found");
            return (true);
        }
    }
    //check if file confi
    if (!file_exits(_path) )
    {
       throw WebServer_Exception("default config file default.conf not found at ./config/");
        return (true);
    }
    return (false);
}