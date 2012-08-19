//==============================================================================
//
// File: CError.h
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
// 
// The CError structure allows for easy error outputting in the CCompiler.
// 
//==============================================================================

#pragma once

#include <string>
#include <list>

struct CError
{
	// The error message
	std::string m_sMessage;
	// The line the error occured on
	int iLine;
};

typedef std::list<CError> ErrorList;