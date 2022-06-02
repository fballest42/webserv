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
    this->_contentType = "Content-Type: ";
    this->_contentLength = "Content-Length: ";
    this->_location = "Location: ";
}

void Response::setHeader()
{
    this->_header = this->_version;
    this->_header += " " + this->_status + "\n";
    this->_header += this->_contentType + "\n";
    //if (m_type != REDIRECT)
    //{
        this->_header += this->_contentLength;
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

void Response::sendResponse(int socket)
{
    //setHeader();
    //setResponse();
    //write(socket, _response.c_str(), _response.length());
    std::string m_body;
    m_body = err_page.get_error_page(400);
    write(socket, m_body.c_str(), m_body.length());
    //close(client_socket);
}

