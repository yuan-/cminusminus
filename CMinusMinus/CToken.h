//==============================================================================
//
// File: CToken.h
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
// 
// This file contains the CToken struct, this structure holds the line the token was
// found on, the value for the token (needed for var names etc), and the actual token type
//
//==============================================================================

#pragma once

#include <string>

// List of all possible tokens
enum eTokenType
{
	// "{"
	OPEN_BRACKET_TOKEN,
	// "}"
	CLOSE_BRACKET_TOKEN,
	// ";"
	SEMICOLON_TOKEN,
	// "="
	EQUALSIGN_TOKEN,
	// "int"
	INTEGER_TYPE_TOKEN,
	// "float"
	FLOAT_TYPE_TOKEN,
	// "string"
	STRING_TYPE_TOKEN,
	// Token for the variable or function name
	VALUE_TOKEN
};

// The token struct
struct CToken
{
	// Which type of token is this?
	eTokenType m_iTokenType;
	// We also save the "value" of the token, for example when we have a VALUE_TOKEN
	// (which means it's either a variable or function name) we want to save the name of this
	// var or function
	std::string m_sValue;
	// Which line of the source was the token found on? Used in the CCMinusMinus, so when we output
	// errors we can also output the line the error occured on
	int m_iLine;
};