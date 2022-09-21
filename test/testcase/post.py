import sys
import os

sys.path.append("../")
from send_request import send_request
import config
import requests

def get_base_url() -> str:
    """
    Return the base URL of the server
    """
    return "http://localhost:{}/".format(config.SERVER_PORT)

# test rfc7230 section 5.4 Host
#1
def test_wrong_post() -> str:
    req = requests.post(get_base_url())
    if req.status_code != 405:
        return "Bad status code: {}, expected: {}".format(req.status_code, 405)
    return ""
#1
def test_post_not_allowed() -> str:
    request_header = "POST / HTTP/1.1\r\nHost: {}\r\n\r\n".format(config.SERVER_NAME)
    http_response = send_request(request_header)
    if http_response.status != 405:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "405"
        )
    return ""
#2
def test_post() -> str:
    request_header = "POST /post/post.html HTTP/1.1\r\nHost: {}\r\nContent-Length: 12\r\n\r\nPOST REQUEST".format(config.SERVER_NAME)
    http_response = send_request(request_header)
    if http_response.status != 201:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "201"
        )
    if http_response.headers["Location"] != "/post/tmp/post.html":
        return "Bad Location header: {}, expected: {}".format(
            http_response.headers["Location"], "/post/tmp/post.html"
        )
    try:
        f = open("www/tmp/post.html", "r")
    except:
        return "Error: file not created"
    line = f.readline()
    #print("content:",line)
    f.close()
    if line != "POST REQUEST":
        return "Bad content: {}, expected: {}".format(line, "POST REQUEST")

    request_header = "POST /post/post.html HTTP/1.1\r\nHost: {}\r\nContent-Length: 21\r\n\r\n ANOTHER POST REQUEST".format(config.SERVER_NAME)
    http_response = send_request(request_header)
    if http_response.status != 200:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "200"
        )
    try:
        f = open("www/tmp/post.html", "r")
    except:
        return "Error: file not created"
    line = f.readline()
    #print(line)
    f.close()
    if line != "POST REQUEST ANOTHER POST REQUEST":
        return "Bad content: {}, expected: {}".format(
            line, "POST REQUEST ANOTHER POST REQUEST"
        )
    #delete file created
    request_header = "DELETE /post/post.html HTTP/1.1\r\nHost: {}\r\n".format(config.SERVER_NAME)
    http_response = send_request(request_header)
    if (http_response.status != 202 and http_response.status != 200):
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "200/202"
    )
    try:
        f = open("www/tmp/post.html", "r")
        f.close()
        return "File still exists after DELETE."
    except:
        return ""
    return ""
#3
def test_too_big_request() -> str:
    payload = "a" * 1000000025
    req = requests.post(get_base_url() + "post/test", data=payload)
    if req.status_code != 413:
        return "Bad status code: {}, expected: {}".format(req.status_code, 413)
    return ""
#4
def test_cgi_headers() -> str:
    request_header = "POST /post/print.cgi HTTP/1.1\r\nHost: {}\r\nEvery-Letter-Should-Change-To_Upcase: HELLOOOOO\r\nContent-Length: 16\r\n\r\nHello from body!\r\n".format(config.SERVER_NAME)
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
    if (
        body.find("HTTP_EVERY_LETTER_SHOULD_CHANGE_TO_UPCASE=HELLOOOOO") == -1
        or 
        body.find("HTTP_HOST=LOCALHOST") == -1
        or body.find("Hello from body!") == -1
    ):
        return "Missing headers from request"
    #print(body)
    return ""
#5
def test_big_post() -> str:
    len =  100000000  # 95M
    request_header = (
        "POST /put/long.txt HTTP/1.1\r\nHost:{}\r\nContent-Length: {}\r\n\r\n{}".format(
            config.SERVER_NAME, len, ("X" * len)
        )
    )
    http_response = send_request(request_header)
    if http_response.status // 100 != 2:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "2XX"
        )
    return ""

def test_cgi_auth_headers() -> str:
    request_header = (
        request_header
    ) = "GET /protected/print.cgi?name=fredrika&age=22 HTTP/1.1\r\nHost:{}\r\nAuthorization: {}\r\n\r\n".format(
        config.SERVER_NAME, config.AUTH_KEY
    )
    http_response = send_request(request_header)
    if http_response.status != 226:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "226"
        )
    body = http_response.read().decode("UTF-8")
    # print(body)
    if (
        body.find("AUTH_TYPE=Basic") == -1
        or body.find("REMOTE_IDENT=Admin") == -1
        or body.find("REMOTE_USER=Admin") == -1
        or body.find("QUERY_STRING=name=fredrika&age=22") == -1
    ):
        return "Missing headers from request"
    # print(body)
    return ""
#6
def test_host3_post() -> str:
    #print("http://{}:{}/post/post.html".format(config.SERVER_NAME3,config.SERVER_PORT3))
    req = requests.post("http://{}:{}/post/post.html".format(config.SERVER_NAME,config.SERVER_PORT3), 
            headers={'Content-Length': '12', 'host': config.SERVER_NAME3},
            data='POST REQUEST')
    if req.status_code != 201:
        return "Bad status code: {}, expected: {}".format(
           str(req.status_code), "201"
        )
    if req.headers["Location"] != "/post/tmp/post.html":
        return "Bad Location header: {}, expected: {}".format(
            req.headers["Location"], "/post/tmp/post.html"
        )
    req = requests.post("http://{}:{}/post/post.html".format(config.SERVER_NAME,config.SERVER_PORT3), 
        headers={'Content-Length': '21', 'host': config.SERVER_NAME3},
        data=' ANOTHER POST REQUEST')
    if req.status_code != 200:
        return "Bad status code: {}, expected: {}".format(
           str(req.status_code), "200"
        )
    if req.headers["Location"] != "/post/tmp/post.html":
        return "Bad Location header: {}, expected: {}".format(
            req.headers["Location"], "/post/tmp/post.html"
        )
    try:
        f = open("www/tmp/post.html", "r")
    except:
        return "Error: file not created"
    line = f.readline()
    #print(line)
    f.close()
    if line != "POST REQUEST ANOTHER POST REQUEST":
        return "Bad content: {}, expected: {}".format(
            line, "POST REQUEST ANOTHER POST REQUEST"
        )
    #delete file created
    req = requests.delete("http://{}:{}/post/post.html".format(config.SERVER_NAME,config.SERVER_PORT3), 
        headers={'host': config.SERVER_NAME3})
    if (req.status_code != 200 and req.status_code != 202):
        return "Bad status code: {}, expected: {}".format(
           str(req.status_code), "200/202"
        )    
    try:
        f = open("www/tmp/post.html", "r")
        f.close()
        return "File still exists after DELETE."
    except:
        return ""
    return ""

#7
def test_host3_cgi() -> str:
    req = requests.post("http://{}:{}/python/py_program.py".format(config.SERVER_NAME,config.SERVER_PORT3), 
        params={'parametro': 'this_is_a_parameter'},
        headers={'Content-Length': '14', 'host': config.SERVER_NAME3},
        data=' POST REQUEST ')
    
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
