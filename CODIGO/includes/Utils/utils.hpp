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
#include <algorithm>

float time_diff2(struct timespec *start, struct timespec *end);

void capitalizeString(const std::string& in, std::string& out);

std::string ft_clean_token(const std::string& in);

bool isSpace(char c);

bool isTab(char c);

void strip(const std::string& in, std::string& out);

void stripTab(const std::string& in, std::string& out);

int nb_tokens(std::string str, char delimiter);

std::string  get_token(std::string str, char delimiter, int nb);

bool file_exits(const std::string name);