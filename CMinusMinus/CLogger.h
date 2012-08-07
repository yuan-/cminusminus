//==============================================================================
//
// File: CLogger.h
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
// 
// This file contains the header file for the logger class. This class is a basic
// wrapper for the std::cout statement, it contains one Write() method, this method
// allows for direct formatting of the string we're outputting instead of using a 
// std::stringstream and then outputting the string.
//
//==============================================================================

#pragma once

#include <iostream>

class CLogger
{
	public:
	// Writes a formatted string to the outputstream
	static void Write(const char * szFormat, ...);
};