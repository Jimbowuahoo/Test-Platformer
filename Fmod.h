#pragma once

#include "fmod.hpp"
#include "fmod.h"
#include <iostream>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <windows.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "fmod_errors.h"

using namespace std;

#pragma comment(lib, "fmodL_vc.lib")
void ERRCHECK_fn(FMOD_RESULT result, const char *file, int line);
#define ERRCHECK(_result) ERRCHECK_fn(_result, __FILE__, __LINE__)

