import sys

sys.path.append("../")
from send_request import send_request
import config
import requests

# test rfc7230 section 5.4 Host
def get_base_url() -> str:
    """
    Return the base URL of the server
    """
    return "http://localhost:{}/".format(config.SERVER_PORT)
#1
def test_get_a() -> str:
    request_header = "GET / HTTP/1.1\r\nHost: {}\r\n\r\n".format(config.SERVER_NAME)
    http_response = send_request(request_header)
    if http_response.status != 200:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "200"
        )
    # check headers
    if http_response.headers["Content-Length"] != "11":
        return "Bad Content-Length: {}, expected: {}".format(
            str(http_response.headers["Content-Length"]), "11")
    if http_response.headers["Content-Type"] != "text/html":
        return "Bad Content-Type: {}, expected: {}".format(
            str(http_response.headers["Content-Type"]), "text/html")
    # check body      
    body = http_response.read().decode("UTF-8")
    index = body.find("hello world")
    if index == -1:
        return "Bad Content: {}, expected: {}".format(
            str(body), "hello world")
    return ""
#2
def test_get_index2() -> str:
    request_header = "GET /second.html HTTP/1.1\r\nHost: {}\r\n\r\n".format(config.SERVER_NAME)
    http_response = send_request(request_header)
    if http_response.status != 200:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "200"
        )
    # check headers
    if http_response.headers["Content-Length"] != "11":
        return "Bad Content-Length: {}, expected: {}".format(
            str(http_response.headers["Content-Length"]), "11")
    if http_response.headers["Content-Type"] != "text/html":
        return "Bad Content-Type: {}, expected: {}".format(
            str(http_response.headers["Content-Type"]), "text/html")
    # check body      
    body = http_response.read().decode("UTF-8")
    index = body.find("second file")
    if index == -1:
        return "Bad Content: {}, expected: {}".format(
            str(body), "second file")
    return ""
#3
def test_get_dir_index() -> str:
    request_header = "GET /a HTTP/1.1\r\nHost: {}\r\n\r\n".format(config.SERVER_NAME)
    http_response = send_request(request_header)
    if http_response.status != 200:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "200"
        )
    body = http_response.read().decode("UTF-8")
    #print(body)
    if body != "hello":
        return "Bad content: {}, expected: {}".format(str(body), "hello")
    return "" 
#4
def test_get_dir_no_index() -> str:
    request_header = "GET /auto HTTP/1.1\r\nHost: {}\r\n\r\n".format(config.SERVER_NAME)
    http_response = send_request(request_header)
    if http_response.status != 404:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "404"
        )
    return ""
#5
def test_get_dir_not_allowed() -> str:
    request_header = "GET /virtual/a/ HTTP/1.1\r\nHost: {}\r\n\r\n".format(config.SERVER_NAME)
    http_response = send_request(request_header)
    if http_response.status != 404:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "404"
        )
    return ""   
#6
def test_custom_404() -> str:
    req = requests.get(get_base_url() + "b")
    if req.status_code != 404:
        return "Bad status code. Recieved: " +  req.status_code + " expected: 404"; 
    if req.text != "custom404":
        return "Bad error page"
    return ""
#7
def test_404() -> str:
    request_header = "GET /doidjodoeijdosejfoejfseoifjseiofjsejfsejfesjfseofsejiseofj HTTP/1.1\r\nHost: {}\r\n\r\n".format(config.SERVER_NAME)
    http_response = send_request(request_header)
    if http_response.status != 404:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "404"
    )
    body = http_response.read() #.decode("UTF-8")
    #print(body)
    return ""
#8
def test_multiple_get() -> str:
    for i in range(100):
        request_header = "GET / HTTP/1.1\r\nHost: {}\r\n\r\n".format(config.SERVER_NAME)
        http_response = send_request(request_header)
        body = http_response.read().decode("UTF-8")
        index = body.find("hello world")
        if http_response.status != 200 or index == -1:
            return "Bad request at {}th iteration.".format(i + 1)
    return ""
#9
def test_different_index() -> str:
    request_header = "GET / HTTP/1.1\r\nHost: {}\r\n\r\n".format(config.SERVER_NAME)
    http_response = send_request(request_header)
    body = http_response.read().decode("UTF-8")
    index = body.find("hello world")
    if http_response.status != 200 or index == -1:
        return "Bad request first index"
    request_header = "GET /2/ HTTP/1.1\r\nHost: {}\r\n\r\n".format(config.SERVER_NAME)
    http_response = send_request(request_header)
    body = http_response.read().decode("UTF-8")
    index = body.find("second file")
    if http_response.status != 200 or index == -1:
        return "Bad request second index"
    return ""
#10
def test_multiple_ports() -> str:
    req = requests.get(get_base_url())
    if req.text != "hello world":
        return "Bad content on first port."
    req = requests.get("http://localhost:{}/".format(config.SERVER_PORT2))
    if req.text != "second file":
        return "Bad content on second port."
    return ""
#11
def test_autoindex() -> str:
    req = requests.get(get_base_url() + "index/a/")
    #print(req.text)
    if req.status_code != 200:
        return "Bad status code : {}, expected: {}".format(
            str(req.status_code), "200")
    #check content
    index = req.text.find("index.html")
    if index == -1:
        return "Bad content in autoindex"
    return ""

def test_content_neg_lang() -> str:
    request_header = (
        "GET /auto/file HTTP/1.1\r\nHost: {}\r\nAccept-Language: fr\r\n\r\n".format(config.SERVER_NAME3)
    )
    print("GET /auto/file HTTP/1.1\r\nHost: {}\r\nAccept-Language: fr\r\n\r\n".format(config.SERVER_NAME3))
    http_response = send_request(request_header)
    if http_response.status != 200:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "200"
        )
    body = http_response.read().decode("UTF-8")
    # print(body)
    if body.find("fr") == -1:
        return "Bad content: {}, expected: {}".format(str(body), "html fr utf-8")
    if http_response.headers["Content-Language"] != "fr":
        return "Missing Content-Language header"
    return ""

def test_content_neg_lang_double() -> str:
    request_header = "GET /auto/file HTTP/1.1\r\nHost: {}\r\nAccept-Language: en-US, fr\r\n\r\n".format(config.SERVER_NAME3)
    http_response = send_request(request_header)
    if http_response.status != 200:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "200"
        )
    body = http_response.read().decode("UTF-8")
    if body.find("fr") == -1:
        return "Bad content: {}, expected: {}".format(str(body), "html fr")
    if http_response.headers["Content-Language"] != "fr":
        return "Missing Content-Language with value of 'fr'"
    return ""

def test_content_neg_charset() -> str:
    request_header = (
        "GET /auto/file HTTP/1.1\r\nHost: {}\r\nAccept-Charset: \r\n\r\n".format(config.SERVER_NAME3)
    )
    http_response = send_request(request_header)
    if http_response.status != 200:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "200"
        )
    body = http_response.read().decode("UTF-8").splitlines()
    if body[0].find("utf-8") != -1:
        return "Bad content: '{}', expected: '{}'".format(str(body[0]), "html")
    return ""

def test_content_neg_charset_utf8() -> str:
    request_header = (
        "GET /auto/file HTTP/1.1\r\nHost: {}\r\nAccept-Charset: utf-8 \r\n\r\n".format(config.SERVER_NAME3)
    )
    http_response = send_request(request_header)
    if http_response.status != 200:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "200"
        )
    body = http_response.read().decode("UTF-8").splitlines()
    if body[0].find("utf-8") == -1:
        return "Bad content: '{}', expected: '{}'".format(str(body[0]), "html xx utf-8")
    if http_response.headers["Content-Type"] != "text/html; charset=utf-8":
        return "Expected Content-Language header with value of 'text/html; charset=utf-8', actual: {}".format(
            http_response.headers["Content-Type"]
        )
    return ""
#12
def test_get_cgi() -> str:
    request_header = "GET /post/print.cgi HTTP/1.1\r\nHost: {}\r\nEvery-Letter-Should-Change-To_Upcase: HELLOOOOO\r\nContent-Length: 16\r\n\r\nHello from body!\r\n".format(config.SERVER_NAME)
    http_response = send_request(request_header)
    if http_response.status != 226:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "226"
        )
    if http_response.headers["Content-Type"] != "CGI/MINE":
        return "Bad content-type: {}, expected: {}".format(
            http_response.headers["Content-Type"], "CGI/MINE"
        )
    body = http_response.read().decode("UTF-8")
    #print(body)
    if (
        body.find("HTTP_EVERY_LETTER_SHOULD_CHANGE_TO_UPCASE=HELLOOOOO") == -1
        or 
        body.find("HTTP_HOST=LOCALHOST") == -1
        or body.find("Hello from body!") == -1
    ):
        return "Missing headers from request"
    #print(body)
    return ""

####### check other servers ##############################
#12
def test_get_host3_dir_index() -> str:
    req = requests.get("http://{}:{}/".format(config.SERVER_NAME,config.SERVER_PORT3),
        headers={'host': config.SERVER_NAME3},
        #params={'Content-Length': '14'},
        #data=' POST REQUEST '
        )
    if req.status_code != 200:
        return "Bad status code: {}, expected: {}".format(
           str(req.status_code), "200"
        )
    if req.text != "google":
        #print (req.text)
        return "Bad content on index."
    return "" 
#13
def test_get_host2_dir_index() -> str:
    req = requests.get("http://{}:{}/".format(config.SERVER_NAME,config.SERVER_PORT2),
        headers={'host': config.SERVER_NAME2},
        #params={'Content-Length': '14'},
        #data=' POST REQUEST '
        )
    if req.status_code != 200:
        return "Bad status code: {}, expected: {}".format(
           str(req.status_code), "200"
        )
    if req.text != "second file":
        #print (req.text)
        return "Bad content on index."
    return "" 

#14
def test_get_host3_dir_autoindex() -> str:
    req = requests.get("http://{}:{}/auto".format(config.SERVER_NAME,config.SERVER_PORT3),
        headers={'host': config.SERVER_NAME3},
        #params={'Content-Length': '14'},
        #data=' POST REQUEST '
        )
    if req.status_code != 200:
        return "Bad status code: {}, expected: {}".format(
           str(req.status_code), "200"
        )
    index = req.text.find("file.html")
    if index == -1:
        return "Bad auto index: {}, expected: {}".format(str(req.text), "file.html")
    return "" 

#15
def test_get_host3_dir_no_index() -> str:
    req = requests.get("http://{}:{}/noindex".format(config.SERVER_NAME,config.SERVER_PORT3),
        headers={'host': config.SERVER_NAME3},
        #params={'Content-Length': '14'},
        #data=' POST REQUEST '
        )
    if req.status_code != 404:
        return "Bad status code: {}, expected: {}".format(
           str(req.status_code), "404"
        )
    #print (req.text)
    return "" 



#16
def test_get_cgi_host3() -> str:
    req = requests.get("http://{}:{}/python/py_program.py".format(config.SERVER_NAME,config.SERVER_PORT3), 
        headers={'host': config.SERVER_NAME3, 'Content-Length': '14'},
        params={'parametro': 'this_is_a_parameter'},
        data=' POST REQUEST '
        )
    
    if req.status_code != 226:
        return "Bad status code: {}, expected: {}".format(
           str(req.status_code), "226"
        )

    body = req.text
    #print(body)
    if (body.find("python") == -1):
        return "Missing body result from request"
    if (body.find("parametro=this_is_a_parameter") == -1):
        return "Missing parameter in body result from request"
    return ""
