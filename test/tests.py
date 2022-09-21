import requests

# from send_request import send_request
import config
import sys

sys.path.append("../")
sys.path.append("../lib")


def get_base_url() -> str:
    """
    Return the base URL of the server
    """
    return "http://localhost:{}/".format(config.SERVER_PORT)


def test_get() -> str:
    req = requests.get(get_base_url())
    print(req.status_code)
    if req.status_code != 200:
        return "Bad status code."
    if req.text != "hello world":
        return "Bad content."
    if req.headers["Content-Length"] != "11":
        return "Bad Content-Length"
    if req.headers["Content-Type"] != "text/html":
         return "Bad Content-Type"
    return ""


def test_two_puts() -> str:
    req = requests.put(get_base_url() + "post/c", data="1")
    if req.status_code != 201:
        return "Bad status code on first creation: {}, expected: {}".format(
            str(req.status_code), "201"
        )
    req = requests.put(get_base_url() + "post/c", data="2")
    if req.status_code != 204:
        return "Bad status code on update: {}, expected: {}".format(
            str(req.status_code), "204"
        )
    return ""

def test_head() -> str:
    req = requests.head(get_base_url())
    if req.status_code != 200:
        return "Bad status code: {}, expected: {}".format(str(req.status_code), "200")
    if len(req.text) > 0:
        return "Head returned some content."
    return ""







#######################


# CGI
# PUT avec chunk
# Multiple GET
# CONNECT
