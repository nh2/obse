#pragma once

#include <string>

bool TestChecksum(const char * procName, std::string * dllSuffix, bool * steamVersion);

// stuff that doesn't belong here
void PrintError(const char * fmt, ...);
std::string GetCWD(void);
