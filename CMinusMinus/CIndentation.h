//==============================================================================
//
// File: CIndentation.h
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
// 
// The CIndentation class holds the indentation level for each variable. The struct
// holds two properties, the indentation level the variable is on, and a unique ID
// for each indentation level. The unique is used in cases like these:
// 
// { int test = 42; } { int bla = test; }
//
// In the above example bla shouldn't be able to access test, even though they're on
// on the same indentation level (1 in our example). We hand each of the enclosing brackets
// a unique ID to prevent this from being allowed. 
// See CParser::HasCorrectIndentationLevel(CIndentation oToBeAccessed, CIndentation oToAccess);
// 
//==============================================================================

#pragma once

#define INVALID_INDENTATION_ID -1
#define INVALID_INDENTATION_LEVEL -1

struct CIndentation
{
	// This variable holds the indentation level the current variable is on
	// Example of a variable on level 0:
	// int test;
	// Example of a variable on level 1:
	// { int test; }
	int m_iLevel;

	// Holds the indentation level ID the variable is on. 
	// Two variables can be on the same levels but not in the same enclosing brackets
	// Example: { int test = 42; } { int bla = test; }, bla shouldn't be able to access test
	// Therefore this variable contains a unique ID for each indentation level
	int m_iLevelID;

	// Default constructor, initialises the variables
	CIndentation::CIndentation(int iLevel, int iLevelID): m_iLevel(iLevel), m_iLevelID(iLevelID) { }
};