//==============================================================================
//
// File: Util.h
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
//
// This file holds a couple of utility functions that may be used across the project.
//
//==============================================================================

#include <string>

// This function returns true if the string input is a float or an integer, false otherwise
bool IsFloatOrInteger(std::string sInput);
// This function returns true if the string input is an integer, false otherwise
bool IsInteger(std::string sInput);