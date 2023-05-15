#pragma once

#ifdef MATHLIBRARY_EXPORTS
#define MATHLIBRARY_API __declspec(dllexport)
#else
#define MATHLIBRARY_API __declspec(dllimport)
#endif

#include <iostream>
#include "windows.h"

using namespace std;
extern "C" MATHLIBRARY_API BOOL InstallHook();
extern "C" MATHLIBRARY_API BOOL UninstallHook();