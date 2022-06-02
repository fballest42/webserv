#include "Response.hpp"
//https://github.com/sqatim/WebServer_42/blob/main/Server/Response.class.cpp

extern Error_page err_page;

Response::Response(/* args */)
{
    initResponse();
}
Response::~Response()
{
}
void Response::initResponse()
{
    //this->_setCookies.clear();
    this->_version = "HTTP/1.1";
    this->_status = "200 OK";
    this->_contentType = "text/html";
    this->_contentLength = 0;
    this->_charset = "UTF-8";
    this->_location = "Location: ";
}

void Response::setHeader()
{
    //header = "HTTP/1.0 200 OK\r\nContent-Length: 11\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"
    this->_header = this->_version;                 //HTTP/1.00  HTTP/1.0_
    this->_header += " " + this->_status + "\r\n";    //200-400... 200 OK\r\n
    this->_header += "Content-Length: " + std::to_string(this->_contentLength) + "\r\n";     //Content-Length: 11
    //headers
    this->_header += "Content-Type: " + this->_contentType + ";";     //Content-Type: text/html
    this->_header += "charset=" + this->_charset + "\r\n\r\n";     //charset=UTF-8\r\n\r\n
    //if (m_type != REDIRECT)
    //{
        //this->_header += this->_contentLength;     //Content-Length: 11

    //}
    /* 
    if (m_type == REDIRECT)
        this->m_header += this->m_location + "\n";
    if (this->m_setCookies.size() > 0)
    {
        this->m_header += "\n";
        for (size_t i = 0; i < this->m_setCookies.size(); i++)
        {
            this->m_header += this->m_setCookies[i];
            if (i + 1 != this->m_setCookies.size())
                this->m_header += "\n";
        }
    }
    */
}

void Response::setResponse()
{

    this->_response = this->_header;
    this->_response += "\n\n";
    //if (m_type != REDIRECT)
        this->_response += this->_body;
}

int Response::sendResponse(int socket)
{
    //setHeader();
    //setResponse();
    // creating a body
    std::string m_body;
    m_body = err_page.get_error_page(999);
    this->_contentLength = m_body.length();
    
    setHeader();
    this->_response = this->_header;
    this->_response += "\n\n";
    this->_response += m_body;
    return (write(socket, this->_response.c_str(), this->_response.length()));
    //close(client_socket);
}

