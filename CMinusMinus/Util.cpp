//==============================================================================
//
// File: Util.cpp
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
//
// This file holds a couple of utility functions that may be used across the project.
//
//==============================================================================

#include "Util.h"

// This function returns true if the string input is a float or integer, false otherwise
bool IsStringFloatOrInteger(std::string sInput)
{
	// This float shouldn't be used for anything else other than being called in sscanf() below
	float fFloatForParsingWithSccanf;

	// sscanf could parse the input as float or int, return true
	if(sscanf_s(sInput.c_str(), "%f", &fFloatForParsingWithSccanf) != 0)
		return true;

	// Otherwise, return false
	return false;
}