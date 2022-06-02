#include "page_errors.hpp"

Error_page::Error_page()
{
    _error_pages[400] = "Bad Request";
    _error_pages[403] = "Forbidden";
    _error_pages[414] = "Url too long";
    _error_pages[501] = "Bad method";
    _error_pages[505] = "Http version not supported";
    _error_pages[999] = "Unidentify error";
};

Error_page::~Error_page()
{
};

std::string Error_page::get_error_page(int error_type){
    
    if (_error_pages.count(error_type)==0)
        error_type = 999;
    //std::string header = "HTTP/1.0 200 OK\r\nContent-Length: 11\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n";
    std::string m_body = "<html>\n";
    m_body += "<head>\n";
    m_body += "<link rel=\"shortcut icon\" href=\"data:image/x-icon;,\" type=\"image/x-icon\"><meta charset=\"UTF-8\">\n";
    m_body += "<title>TEST PAGE1</title>\n</head>";
    m_body += "<center><h1>";
    m_body += _error_pages[error_type];
    m_body += "</h1></center>\n";
    m_body += "<hr><center>dperez42</center>\n";
    m_body += "</html>";
    return m_body;
};