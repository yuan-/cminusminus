//==============================================================================
//
// File: CTokenizer.h
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
//
// The tokenizer reads the entire source input and splits it up in tokens (without
// validating anything though). It contains a method which -after parsing- returns
// the list of tokens.
//
//==============================================================================

#pragma once

#include "CToken.h"
#include <string>

class CTokenizer
{
	// The token list
	TokenList m_lTokenList;
	// The name of the source file we're supposed to parse
	std::string m_sSourceFile;

public:
	// The constructor of the CTokenizer class
	CTokenizer(std::string sSourceInput);
	// Parses the source file
	void Run();
	// Pushes a token onto the token list
	void AddTokenToList(std::string sTokenValue, int iLineNumber, CIndentation oIndentation);
	// Pushes a string literal onto the token list
	void AddStringLiteralToList(std::string sValue, int iLineNumber, CIndentation oIndentation);
	// Returns the token type from the value
	eTokenType GetTokenType(std::string sTokenValue);
	// Returns the token list
	TokenList GetTokenList();
	// This method returns the string type from the token, this method is only available when compiling in debug mode
	#if _DEBUG
	const char * getStringFromTokenType(eTokenType eType);
	#endif
};