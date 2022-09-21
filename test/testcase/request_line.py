import sys

sys.path.append("../")
from send_request import send_request
import config

# test rfc7230 section 3.3.1: Request Line

#1
def test_http_version() -> str:
    request_header = 'GET / HTTP/0.1\r\nHost:{}\r\n\r\n'.format(
        config.SERVER_NAME)
    http_response = send_request(request_header)
    if http_response.status != 505 and http_response.status // 100 != 4:
        return "Bad status code: {}, expected: {}".format(str(http_response.status), '505 or 4XX')
    return ""
#2
def test_request_line_multiple_space() -> str:
    request_header = "GET /   HTTP/1.1\r\nHost:{}\r\n\r\n".format(config.SERVER_ADDR)
    http_response = send_request(request_header)
    if http_response.status != 400:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "400"
        )
    return ""
#3
def test_request_line_too_long() -> str:
    target = "/" + "A" * (config.MAX_URI_LENGTH - 1)
    request_header = "GET {} HTTP/1.1\r\nHost:{}\r\n\r\n".format(
        target, config.SERVER_ADDR
    )
    http_response = send_request(request_header)
    if http_response.status != 414:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "400"
        )
    return ""
