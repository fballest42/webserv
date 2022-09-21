"""
The run_test function is a wrapper for your test.
Your test should be a function that takes a port as an int, and returns a string, empty if the test is
successfull, containing the reason of the failure if failed.
"""
from cgi import print_exception
import os
import sys

from send_request import send_request
from testcase.content_length import *
from testcase.header_field import *
from testcase.host import *
from testcase.request_line import *
#from testcase.http_version_not_supported import test_http_version
from testcase.get import *
from testcase.post import *
from testcase.put import *
from testcase.chunked import *
from testcase.delete import *

from typing import Callable
from tests import *
import config

# import colors
# color letters
RESET = "\033[0m"

C_BLACK = "\033[30m"
C_RED = "\033[31m"
C_GREEN = "\033[32m"
C_YELLOW = "\033[33m"
C_BLUE = "\033[34m"
C_MAGENTA = "\033[35m"
C_CYAN = "\033[36m"
C_WHITE = "\033[37m"
C_GRAY = "\033[90m"
# color letters Gras
C_B_BLACK = "\033[30;01m"
C_B_RED = "\033[31;01m"
C_B_GREEN = "\033[32;01m"
C_B_YELLOW = "\033[33;01m"
C_B_BLUE = "\033[34;01m"
C_B_MAGENTA = "\033[35;01m"
C_B_GRAY = "\033[36;01m"
C_B_WHITE = "\033[37;01m"
C_B_GRAY = "\033[90;01m"
# color background
B_BLACK = "\033[40m"
B_RED = "\033[41m"
B_GREEN = "\033[42m"
B_YELLOW = "\033[43m"
B_BLUE = "\033[44m"
B_MAGENTA = "\033[45m"
B_GRAY = "\033[46m"
B_WHITE = "\033[47m"
B_GRAY = "\033[100m"


def cleanup() -> None:
    """
    Remove file created by the tester to make sure the test is new every run.
    """
    os.system("rm -rf www/tmp/*")
    os.system("rm -rf www/long.txt")


def run_test(test_name: str, test: Callable) -> None:
    """
    Runs a test defined in function test, with the name test_name,
    and prints wether it passed or not.
    """
    try:
        result = test()
    except Exception as e: 
        print(e)
        print(
            "{}Cannot connect to the server on port {}{}".format(
                C_B_RED, config.SERVER_PORT, RESET
            )
        )
        exit(1)
    char = ""
    color = C_GREEN
    if len(result) == 0:
        char = "✅"
    else:
        color = C_RED
        char = "❌"
    print(r"{:40} {}{} {}{}".format(test_name, color, char, result, RESET))


def run() -> None:
    """
    Entrypoint of the tester
    """

    print(r"{}{}### TESTING REQUEST LINE ###{}".format(C_B_WHITE, B_GRAY, RESET))
    run_test("1 Http Version: GET / HTTP/0.1", test_http_version)
    run_test("2 GET  / HTTP/1.1", test_request_line_multiple_space)
    print("3 test_request_line_too_long timeout?")
    run_test("3 GET /{A*10^10} HTTP/1.1", test_request_line_too_long) #time out?

    print(r"{}{}### TESTING HEADERS ###{}".format(C_B_WHITE, B_GRAY, RESET))
    run_test("1 Testing mandatory headers", test_headers)
    # #run_test("2 Testing protected realm", test_auth_no_key)
    # #run_test("3 Testing protected realm access", test_auth)
    run_test("4 Testing case insensitive", test_case_insensitive)

    print(r"{}1. testing headers{}".format(C_CYAN, RESET))
    run_test("5 Test Space_Before_Colon 'GET / HTTP/1.1\\r\\nHost :'", test_space_before_colon)
    run_test("6 GET / HTTP/1.1\\r\\n: empty", test_missing_header_name)
    run_test("7 GET / HTTP/1.1\\r\\nUser-Agent: {A*10^11}", test_header_too_long)

    print(r"{}2. testing host header:{}".format(C_CYAN, RESET))
    run_test("8 Test_Missing_Host: GET / HTTP/1.1\\r\\n\\r\\n", test_missing_host)
    run_test("9 Test_Double_Host: naver.com\\r\\nHost: hyeyoo.com", test_double_host)
    run_test("10 Test_Multiple_Host: localhost\\r\\nHost: localhost", test_multiple_host)
    run_test("11 Test_Invalid_Host: hyeyoo@hyeyoo.com", test_invalid_host)
    run_test("12 Test_Valid_Host: google1.com", test_valid_host3)
    run_test("13 Test_Valid_Host: default_server", test_valid_host2)

    print(r"{}3. testing content-length header{}".format(C_CYAN, RESET))
    run_test("12 Test_Neg_Content-Length: -1", test_neg_content_length)
    run_test("13 Test Big Content-Length: 10^20", test_big_content_length)
    run_test("14 Test Alpha Content-Length: NOTDIGIT", test_alpha_content_length)
    run_test("15 Test Content-Length & Chunked", test_length_and_chunked)
    run_test("16 Test Double Content-Length header", test_double_length)

    print(r"{}{}### TESTING CHUNKED POST###{}".format(C_B_WHITE, B_GRAY, RESET))
    run_test("Testing read content & trailer POST /post/print.cgi HTTP/1.1 Host: localhost Transfer-encoding: chunked" , test_chunked_w_trailer)
    run_test("1 POST /post/print.cgi HTTP/1.1 Host: localhost Transfer-encoding: chunked 4 test 00000", test_chunked_multiplezeros)
    run_test("2 POST /post/print.cgi HTTP/1.1 Host: localhost Transfer-encoding: chunked 4 test 000; dude", test_ChunkExtensionNoValue)
    run_test("3 POST /post/print.cgi HTTP/1.1 Host: localhost Transfer-encoding: chunked 4 test 000; dude=best ", test_ChunkExtensionUnquotedValue)
    run_test("4 POST /post/print.cgi HTTP/1.1 Host: localhost Transfer-encoding: chunked 4 test 000; dude=best ;Foo=Bar; dude='The dude is the best';Spam=12345!", test_MultipleChunkExtensions)
    run_test("5 POST /post/print.cgi HTTP/1.1 Host: localhost Transfer-encoding: chunked 0 r n r n", test_DecodeEmptyBodyInPieces)
    run_test("6 POST /post/print.cgi HTTP/1.1 Host: localhost Transfer-encoding: chunked 0\\r\\n\\r\\nHello!", test_DecodeEmptyBodyWithExtraStuffAfter)
    run_test("7 POST /post/print.cgi HTTP/1.1 Host: localhost Transfer-encoding: chunked F\\r\\nHello, World!!!\\r\\n7\\r\\nIt's me\\r\\n0\\r\\n\\r\\n", test_DecodeThreeChunksOnePiece)
    run_test("8 POST /post/print.cgi HTTP/1.1 Host: localhost Transfer-encoding: chunked 0\\r\\nX-Foo Bar", test_DecodeBadTrailer)

    print(r"{}{}### TESTING GET ###{}".format(C_B_WHITE, B_GRAY, RESET))
    run_test("1 Test Get_/_default_index: GET / HTTP/1.1", test_get_a)
    run_test("2 Test Get_/_second_index: GET /second.html HTTP/1.1", test_get_index2)
    run_test("3 Test Get_dir_default_index: GET /a", test_get_dir_index)
    run_test("4 Test Get_dir_no_index: GET /auto ", test_get_dir_no_index)
    run_test("5 Test Get_dir_not_allowed: GET /virtual/a/", test_get_dir_not_allowed)
    run_test("6 Test Custom_error_not_found: GET /b, checking custom 404 page", test_custom_404)
    run_test("7 Test Default_error_not_found: GET /doidjo...", test_404)  #se heredan los ?????
    run_test("8 Test Multiple_gets_100: GET /", test_multiple_get)
    run_test("9 Test Get_different_index: GET / y GET /2/", test_different_index)
    run_test("10 Test Get_/_ports_8000y8081: GET / on port 8080 and 8081", test_multiple_ports)
    run_test("11 Test Autoindex: GET /index/a/   check autoindex", test_autoindex)
    run_test("12 Test Get_cgi'c': GET /post/print.cgi?par=a ", test_get_cgi)

    # # ########## CHECK IDIOM OR CHARSHET ############
    # # run_test("GET /auto/file, Accept-Language: fr", test_content_neg_lang)
    # # run_test("/auto/file, Accept-Language: en-US, fr", test_content_neg_lang_double)
    # # run_test("/auto/file, Accept-Charset: ", test_content_neg_charset)
    # # run_test("/auto/file, Accept-Charset: utf-8", test_content_neg_charset_utf8)

    # ##### CHECK OTHER HOST ############
    run_test("12 Test Get_google1.com: GET /, Host: google1.com", test_get_host3_dir_index)
    run_test("13 Test Get_default_server: GET /, Host: default_server", test_get_host2_dir_index)
    run_test("14 Test Get_dir_autoindex_goole1.com: GET /auto, Host: google1.com", test_get_host3_dir_autoindex)
    run_test("15 Test Get_dir_no_index_google1.com: GET /auto, Host: google1.com", test_get_host3_dir_no_index)
    run_test("16 Test Get_cgi'python' Host3: GET /python/pu_program.py?parametro=this_is_a_parameter ", test_get_cgi_host3)
    
    print(r"{}{}### TESTING POST ###{}".format(C_B_WHITE, B_GRAY, RESET))
    run_test("1 Test Post_Not_allowed_method: POST /", test_wrong_post)
    run_test("1 Test Post_Not_allowed_method: POST / HTTP/1.1 Host: xx", test_post_not_allowed)
    run_test("2 Test Post_upload_dir_file (2 files): POST /post/post.html * 2", test_post)
    run_test("3 POST /post/test too big payload", test_too_big_request)
    run_test("4 Test Post_cgi'c': POST /post/print.cgi ", test_cgi_headers)
    run_test("5 POST /a/long.txt", test_big_post)
    # ##### CHECK OTHER HOST ############
    run_test("6 Test Post_Host3_upload_dir_file (2 files): POST /post/post.html * 2", test_host3_post)
    run_test("7 Test Post_Host3_CGI_file_params: POST /python/py_program.py?pparametro:this_is_a_parameter", test_host3_cgi)

    print(r"{}{}### TESTING DELETE ###{}".format(C_B_WHITE, B_GRAY, RESET))
    run_test("1 Test Post and Delete a file: POST AND DELETE /post/tmp.txt ", test_delete)
    run_test("2 Test Delete_not_existing_file: DELETE /post/notexits.txt", test_delete_no_exits_file)
    run_test("3 Test Delete_dir: DELETE /post/gone", test_delete_no_file)
    # ##### CHECK OTHER HOST ############
    run_test("4 Test Post and Delete a file Host3: POST AND DELETE /tmp.txt ", test_host3_delete)
    run_test("5 Test Delete_not_existing_file Host3: DELETE /notexits.txt", test_delete_host3_no_exits_file)
    run_test("6 Test Delete_dir Host3: DELETE /notexits/", test_delete_host3_no_file)
##--------------------------------------------##

    # print(r"{}{}### TESTING HEAD ###{}".format(C_B_WHITE, B_GRAY, RESET))
    # run_test("HEAD /", test_head)
    # run_test("PUT /a/long.txt", test_big_put)
    # run_test("GET /protected/print.cgi ", test_cgi_auth_headers)


    # print(r"{}{}### TESTING PUT ###{}".format(C_B_WHITE, B_GRAY, RESET))
    # run_test("PUT / HTTP/1.1\r\nHost: xx\r\n\r\n", test_put_not_allowed)
    #run_test("PUT /post/put.html HTTP/1.1\r\nHost: xx\r\nContent-Length: 11\r\n\r\nPUT REQUEST", test_put)
    # run_test("PUT /a/long.txt", test_big_put)

    # run_test("PUT /post/c * 2", test_two_puts)

    print(r"{}{}### TESTING DELETE ###{}".format(C_B_WHITE, B_GRAY, RESET))
    run_test("PUT /post/test, DELETE /post/test", test_delete)
    # 

if __name__ == "__main__":
    cleanup()
    run()
