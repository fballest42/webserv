#include "page_errors.hpp"

extern Logger log;

Error_page::Error_page()
{
    // INFORMATIONAL
    _error_pages[100] = "Continue";
    _error_pages[101] = "Switching Protocol";
    _error_pages[102] = "Processing (WebDAV)";
    _error_pages[103] = "Early Hints";
    // SUCCESS
    _error_pages[200] = "OK";
    _error_pages[201] = "Created";
    _error_pages[202] = "Accepted";
    _error_pages[203] = "Non-Authoritative Information";
    _error_pages[204] = "No Content";
    _error_pages[205] = "Reset Content";
    _error_pages[206] = "Partial Content";
    _error_pages[207] = "Multi-Status";
    _error_pages[208] = "Already Reported";
    _error_pages[226] = "IM Used";
    // REDIRECTION
    _error_pages[300] = "Multiple Choice";
    _error_pages[301] = "Moved Permanently";
    _error_pages[302] = "Found";
    _error_pages[303] = "See Other";
    _error_pages[304] = "Not Modified";
    _error_pages[305] = "Use Proxy";
    _error_pages[307] = "Temporary Redirect";
    _error_pages[308] = "Permanent Redirect";
    //Error de cliente
    _error_pages[400] = "Bad Request";
    _error_pages[401] = "Unauthorized";
    _error_pages[402] = "Payment Required";
    _error_pages[403] = "Forbidden";
    _error_pages[404] = "Not Found";
    _error_pages[405] = "Method Not Allowed";
    _error_pages[406] = "Not Acceptable";
    _error_pages[407] = "Proxy Authentication Required";
    _error_pages[408] = "Request Timeout";
    _error_pages[409] = "Conflict";
    _error_pages[410] = "Gone";
    _error_pages[411] = "Length Required";
    _error_pages[412] = "Precondition Failed";
    _error_pages[413] = "Payload Too Large";
    _error_pages[414] = "URI too long";
    _error_pages[415] = "Unsupported Media Type";
    _error_pages[416] = "Requested Range Not Satisfiable";
    _error_pages[417] = "Expectation Failed";
    _error_pages[418] = "I'm a teapot";
    _error_pages[421] = "Misdirected Request";
    _error_pages[422] = "Unprocessable Entity (WebDAV)";
    _error_pages[423] = "Locked (WebDAV)";
    _error_pages[424] = "Failed Dependency (WebDAV)";
    _error_pages[425] = "Too Early";
    _error_pages[426] = "Upgrade Required";
    _error_pages[428] = "Precondition Required";
    _error_pages[429] = "Too Many Request";
    _error_pages[431] = "Request Headers Fields Too Large";
    _error_pages[451] = "Unavailable For Legal Reasons";
    _error_pages[499] = "Client Closed Request";
    //Errores de Servidor
    _error_pages[500] = "Internal Server Error";
    _error_pages[501] = "Not implemented";
    _error_pages[502] = "Bad Gateway";
    _error_pages[503] = "Service Unavailable";
    _error_pages[504] = "Gateway Timeout";
    _error_pages[505] = "Http version not supported";
    _error_pages[506] = "Variant Also Negotiates";
    _error_pages[507] = "Insufficient Storage";
    _error_pages[508] = "Loop Detected";
    _error_pages[510] = "Not Extended";
    _error_pages[511] = "Network Authentication Requiered";
    _error_pages[599] = "Network Connect Timeout Error";
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
    m_body += "<head><title>" + std::to_string(error_type) + " " + _error_pages[error_type] + "</title></head>\n";
    //m_body += "<link rel=\"shortcut icon\" href=\"data:image/x-icon;,\" type=\"image/x-icon\"><meta charset=\"UTF-8\">\n";
    m_body += "<body>\n";
    m_body += "<center><h1>";
    m_body += std::to_string(error_type) + " " + _error_pages[error_type];
    m_body += "</h1></center>\n";
    m_body += "<hr><center>dperez-z and fballest webserv</center>\n";
    m_body += "</body>\n";
    m_body += "</html>";
    log.print(INFO,"Error page create: " + std::to_string(error_type) + " " + _error_pages[error_type],RED,true);
    return m_body;
};

std::string Error_page::get_error_description(int error_type)
{
    return _error_pages.at(error_type);
}