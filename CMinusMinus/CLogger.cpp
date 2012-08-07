//==============================================================================
//
// File: CLogger.cpp
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
//
// This file contains the logger class. This class logs data to a log file.
//
//==============================================================================

#include "CLogger.h"

void CLogger::Write(const char * szFormat, ...)
{
	va_list vaArgs;
	char szBuffer[2048];
	_crt_va_start(vaArgs, szFormat);
	vsnprintf_s(szBuffer, sizeof(szBuffer), szFormat, vaArgs);
	_crt_va_end(vaArgs);

	// Print the arguments to the console
	std::cout << szBuffer << std::endl;
}