#pragma once

#include <vector>
#include <map>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <stdio.h>      
#include <stdlib.h> 
#include <cstring>  
#include <sstream>
#include <string> 
#include <fcntl.h>
#include <sys/stat.h>

bool isSpace(char c);

void strip(const std::string& in, std::string& out);

int nb_tokens(std::string str, char delimiter);

std::string  get_token(std::string str, char delimiter, int nb);

bool file_exits(const std::string name);