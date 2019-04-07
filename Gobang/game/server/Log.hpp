#pragma once

#include <iostream>
#include <vector>
#include <string>

#define INFO 0
#define WARNING 1
#define FATAL 2

using namespace std;

vector<string> g_error_level={
    "INFO",
    "WARNING",
    "FATAL",
};

void Log(int level, std::string message, std::string file, int line)
{
    cout << "[ " << g_error_level[level] << " ][ " << message << " ] : " << file << " : " << line << endl;
}

#define LOG(level, message) Log(level, message, __FILE__, __LINE__)












