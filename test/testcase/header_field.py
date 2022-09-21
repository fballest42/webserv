import sys

sys.path.append("../")
import config
from send_request import send_request
import requests

# test rfc7230 section 3.2.4: Field Parsing

#1
def test_headers() -> str:
    request_header = "GET / HTTP/1.1\r\nHost:{}\r\n\r\n".format(config.SERVER_NAME)
    http_response = send_request(request_header)
    if http_response.status != 200:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "200"
        )
    headers = http_response.headers
    #print(headers)
    if (
        headers["Content-Length"] == None
        or headers["Date"] == None
        or headers["Server"] == None
        or headers["Last-Modified"] == None
    ):
        return "Missing headers"
    return ""

#2
def test_auth_no_key() -> str:
    request_header = "GET /protected HTTP/1.1\r\nHost:{}\r\n\r\n".format(
        config.SERVER_NAME
    )
    http_response = send_request(request_header)
    if http_response.status != 401:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "401"
        )
    if http_response.headers["WWW-Authenticate"] == None:
        return "Missing 'WWW-Authenticate' header"
    return ""

#3
def test_auth() -> str:
    request_header = (
        "GET /protected HTTP/1.1\r\nHost:{}\r\nAuthorization: {}\r\n\r\n".format(
            config.SERVER_NAME, config.AUTH_KEY
        )
    )
    http_response = send_request(request_header)
    if http_response.status != 200:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "200"
        )
    return ""

#4
def test_case_insensitive() -> str:
    request_header = "GET / HTTP/1.1\r\nhost:{}\r\n\r\n".format(config.SERVER_NAME)
    http_response = send_request(request_header)
    if http_response.status != 200:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "200"
        )
    # request_header = (
    #     "GET /protected HTTP/1.1\r\nHost:{}\r\nautHORIzatiON: {}\r\n\r\n".format(
    #         config.SERVER_NAME, config.AUTH_KEY
    #     )
    # )
    # http_response = send_request(request_header)
    # if http_response.status != 200:
    #     return "Bad status code: {}, expected: {}".format(
    #         str(http_response.status), "200"
    #     )
    return ""

#5
def test_space_before_colon() -> str:
    request_header = "GET / HTTP/1.1\r\nHost :{}\r\n\r\n".format(config.SERVER_NAME)
    http_response = send_request(request_header)
    if http_response.status != 400:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "400"
        )
    request_header = (
        "GET / HTTP/1.1\r\nHost:{}\r\nAccept-Language :hyeyoo\r\n\r\n".format(
            config.SERVER_NAME
        )
    )
    http_response = send_request(request_header)
    if http_response.status != 400:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "400"
        )
    return ""

#6
def test_missing_header_name() -> str:
    request_header = "GET / HTTP/1.1\r\nHost:{}\r\n:empty_name\r\n\r\n".format(
        config.SERVER_NAME
    )
    http_response = send_request(request_header)
    if http_response.status // 100 != 4:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "4XX"
        )
    return ""

#7
def test_header_too_long() -> str:
    long_text = "T" * 100000
    request_header = (
        "GET / HTTP/1.1\r\nHost: {}\r\nUser-Agent: {}\r\n\r\n".format(config.SERVER_NAME,long_text)
    )
    http_response = send_request(request_header)
    # print(http_response.status)
    if http_response.status // 100 != 4:
        return "Bad status code: {}, expected: {} (NOT MANDATORY)".format(
            str(http_response.status), "4XX"
        )
    return ""

# test rfc7230 section 5.4 Host
#8
def test_missing_host() -> str:
    req = requests.get("http://{}:{}/".format(config.SERVER_NAME,config.SERVER_PORT3), 
        headers={'Host': ''},
        #params={'Content-Length': '14'},
        #data=' POST REQUEST '
    )
    if req.status_code != 400:
        return "Bad status code: {}, expected: {}".format(
           str(req.status_code), "400"
        )
    return ""

#9
def test_double_host() -> str:
    req = requests.get("http://{}:{}/".format(config.SERVER_NAME,config.SERVER_PORT), 
        headers={'host': 'naver.com , hyeyoo.com'},
        #params={'Content-Length': '14'},
        #data=' POST REQUEST '
        )
    if req.status_code != 400:
        return "Bad status code: {}, expected: {}".format(
           str(req.status_code), "400"
        )
    return ""

#10
def test_multiple_host() -> str:
    req = requests.get("http://{}:{}/".format(config.SERVER_NAME,config.SERVER_PORT), 
        headers={'host': 'localhost , localhost'},
        #params={'Content-Length': '14'},
        #data=' POST REQUEST '
        )
    if req.status_code != 400:
        return "Bad status code: {}, expected: {}".format(
           str(req.status_code), "400"
        )
    return ""

#11
def test_invalid_host() -> str:
    req = requests.get("http://{}:{}/".format(config.SERVER_NAME,config.SERVER_PORT), 
        headers={'host': 'hyeyoo@hyeyoo.com'},
        #params={'Content-Length': '14'},
        #data=' POST REQUEST '
        )
    if req.status_code != 400:
        return "Bad status code: {}, expected: {}".format(
           str(req.status_code), "400"
        )
    # request_header = "GET / HTTP/1.1\r\nHost: hyeyoo@hyeyoo.com\r\n\r\n"
    # http_response = send_request(request_header)
    # if http_response.status != 400:
    #     return "Bad status code: {}, expected: {}".format(
    #         str(http_response.status), "400"
    #     )
    return ""
#12
def test_valid_host2() -> str:
    req = requests.get("http://{}:{}/".format(config.SERVER_NAME,config.SERVER_PORT3), 
        headers={'host': config.SERVER_NAME3},
        #params={'Content-Length': '14'},
        #data=' POST REQUEST '
        )
    if req.status_code != 200:
        return "Bad status code: {}, expected: {}".format(
           str(req.status_code), "200"
        )
    return ""
#13
def test_valid_host3() -> str:
    req = requests.get("http://{}:{}/".format(config.SERVER_NAME,config.SERVER_PORT3), 
        headers={'host': config.SERVER_NAME3},
        #params={'Content-Length': '14'},
        #data=' POST REQUEST '
        )
    if req.status_code != 200:
        return "Bad status code: {}, expected: {}".format(
           str(req.status_code), "200"
        )
    return ""

# test rfc7230 section 3.3.2: Content Length

#12
def test_neg_content_length() -> str:
    length = "-1"
    request_header = "GET / HTTP/1.1\r\nHost:{}\r\nContent-Length: {}\r\n\r\n".format(
        config.SERVER_NAME, length
    )
    http_response = send_request(request_header)
    if http_response.status != 400:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "400"
        )
    return ""

#13
def test_big_content_length() -> str:
    length = "100000000000000000000000"
    request_header = "GET / HTTP/1.1\r\nHost:{}\r\nContent-Length: {}\r\n\r\n".format(
        config.SERVER_NAME, length
    )
    http_response = send_request(request_header)
    if http_response.status != 400:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "400"
        )
    return ""

#14
def test_alpha_content_length() -> str:
    length = "NOTDIGIT"
    request_header = "GET / HTTP/1.1\r\nHost:{}\r\nContent-Length: {}\r\n\r\n".format(
        config.SERVER_NAME, length
    )
    http_response = send_request(request_header)
    if http_response.status != 400:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "400"
        )
    return ""

#15
def test_length_and_chunked() -> str:
    request_header = "GET / HTTP/1.1\r\nHost:{}\r\nContent-Length: 10000\r\nTransfer-Encoding: chunked\r\n\r\n0\r\n\r\n".format(
        config.SERVER_NAME
    )
    http_response = send_request(request_header)
    if http_response.status != 200:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "200"
        )
    return ""

#16
def test_double_length() -> str:
    request_header = "GET / HTTP/1.1\r\nHost:{}\r\nContent-Length: 1\r\nContent-Length: 0\r\n\r\n".format(
        config.SERVER_NAME
    )
    http_response = send_request(request_header)
    if http_response.status != 400:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "400"
        )
    return ""





