import requests
from send_request import send_request

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
#1
def test_delete() -> str:
    req = requests.post("http://{}:{}/post/tmp.txt".format(config.SERVER_NAME,config.SERVER_PORT), 
            headers={'Content-Length': '12', 'host': config.SERVER_NAME},
            data='POST  DELETE')
    if req.status_code != 201:
        return "Bad status code: {}, expected: {}".format(
           str(req.status_code), "201"
        )
    if req.headers["Location"] != "/post/tmp/tmp.txt":
        return "Bad Location header: {}, expected: {}".format(
            req.headers["Location"], "/post/tmp/tmp.txt"
        )
    try:
        f = open("www/tmp/tmp.txt", "r")
    except:
        return "Error: file not created"
    line = f.readline()
    f.close()
    if line != "POST  DELETE":
        return "Bad content: {}, expected: {}".format(line, "POST  DELETE")

    req = requests.delete("http://{}:{}/post/tmp.txt".format(config.SERVER_NAME,config.SERVER_PORT), 
        headers={'host': config.SERVER_NAME})
    if (req.status_code != 200 and req.status_code != 202):
        return "Bad status code: {}, expected: {}".format(
           str(req.status_code), "200/202"
        )    
    try:
        f = open("www/tmp/tmp.txt", "r")
        f.close()
        return "File still exists after DELETE."
    except:
        return ""
    return ""
#2
def test_delete_no_exits_file() -> str:
    req = requests.delete("http://{}:{}/post/notexits.txt".format(config.SERVER_NAME,config.SERVER_PORT), 
        headers={'host': config.SERVER_NAME})
    if req.status_code != 404:
        return "Bad status code: {}, expected: {}".format(
           str(req.status_code), "404")
    return ""
#3
def test_delete_no_file() -> str:
    req = requests.delete("http://{}:{}/post/gone".format(config.SERVER_NAME,config.SERVER_PORT), 
        headers={'host': config.SERVER_NAME})
    #req = requests.delete(get_base_url() + "post/gone")
    if req.status_code != 404:
        return "Bad status code: {}, expected: {}".format(
           str(req.status_code), "404")
    return ""

#4
def test_host3_delete() -> str:
    req = requests.post("http://{}:{}/tmp.txt".format(config.SERVER_NAME,config.SERVER_PORT3), 
        headers={'Content-Length': '12','host': config.SERVER_NAME3},
        data='POST REQUEST')
    if (req.status_code!= 201):
        return "Bad status code: {}, expected: {}".format(
            str(req.status_code), "201"
    )
    try:
        f = open("www/tmp/tmp.txt", "r")
    except:
        return "Error: file not created"
    req = requests.delete("http://{}:{}/tmp.txt".format(config.SERVER_NAME,config.SERVER_PORT3), 
        headers={'Content-Length': '12','host': config.SERVER_NAME3},
        data='POST REQUEST')
    if (req.status_code!= 202 and req.status_code != 200):
        return "Bad status code: {}, expected: {}".format(
            str(req.status_code), "200/202"
    )
    try:
        f = open("www/tmp/tmp.txt", "r")
        f.close()
        return "File still exists after DELETE."
    except:
        return ""
    return ""
#5
def test_delete_host3_no_exits_file() -> str:
    req = requests.delete("http://{}:{}/notexits.txt".format(config.SERVER_NAME,config.SERVER_PORT3),
        headers={'Content-Length': '12', 'host': config.SERVER_NAME3},
        data='POST REQUEST')
    if (req.status_code!= 404):
        return "Bad status code: {}, expected: {}".format(
            str(req.status_code), "404"
    )
    return ""
#6
def test_delete_host3_no_file() -> str:
    req = requests.delete("http://{}:{}/notexits/".format(config.SERVER_NAME,config.SERVER_PORT3),
        headers={'Content-Length': '12', 'host': config.SERVER_NAME3},
        data='POST REQUEST')
    if (req.status_code!= 404):
        return "Bad status code: {}, expected: {}".format(
            str(req.status_code), "404"
    )
    return ""
