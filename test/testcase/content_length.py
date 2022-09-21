import sys

sys.path.append("../")
import config
from send_request import send_request

# test rfc7230 section 3.3.2: Content Length

def test_big_put() -> str:
    len = 100000000  # 10M
    request_header = (
        "PUT /put/long.txt HTTP/1.1\r\nHost:{}\r\nContent-Length: {}\r\n\r\n{}".format(
            config.SERVER_NAME, len, ("X" * len)
        )
    )
    http_response = send_request(request_header)
    if http_response.status != 201:
        return "Bad status code: {}, expected: {}".format(
            str(http_response.status), "201"
        )
    return ""


