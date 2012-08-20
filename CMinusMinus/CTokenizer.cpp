//==============================================================================
//
// File: CTokenizer.cpp
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
//
// The tokenizer reads the entire source input and splits it up in tokens (without
// validating anything though). It contains a method which -after parsing- returns
// the list of tokens.
//
//==============================================================================

#include "CToken.h"
#include "CTokenizer.h"
#include "CLogger.h"

#include <fstream>

// The constructor of the CTokenizer class
CTokenizer::CTokenizer(std::string sSourceFile)
{
	m_sSourceFile = sSourceFile;
}

// Returns the token type from the token string
eTokenType CTokenizer::GetTokenType(std::string sTokenValue)
{
	if(sTokenValue == ";")
		return SEMICOLON_TOKEN;
	if(sTokenValue == "{")
		return OPEN_BRACKET_TOKEN;
	if(sTokenValue == "}")
		return CLOSE_BRACKET_TOKEN;
	if(sTokenValue == "int")
		return INTEGER_TYPE_TOKEN;
	if(sTokenValue == "float")
		return FLOAT_TYPE_TOKEN;
	if(sTokenValue == "string")
		return STRING_TYPE_TOKEN;
	if(sTokenValue == "=")
		return EQUALSIGN_TOKEN;

	// No valid token found, this must be a function or variable name
	return VALUE_TOKEN;
}

// This method pushes a new token onto the token list
void CTokenizer::AddTokenToList(std::string sTokenValue, int iLineNumber)
{
	// Create a new CToken object
	CToken oToken;
	oToken.m_iTokenType = this->GetTokenType(sTokenValue);
	oToken.m_iLine = iLineNumber;
	oToken.m_sValue = sTokenValue;

	// Push the object back on the list
	m_lTokenList.push_back(oToken);
}

// Run the tokenizer, this method parses the source file and pushes all
// found tokens on to the token list
void CTokenizer::Run()
{
	// This string will hold the current value for the token
	std::string sTokenValue;
	// This string will hold the current source line in the while() loop
	std::string sLine;
	// The current source line number
	int iLineNumber = 1;

	std::ifstream fileStream(m_sSourceFile);

	// Was the source file correctly opened?
	if(!fileStream.is_open())
	{
		CLogger::Write("* Could not open source file %s", m_sSourceFile.c_str());
		exit(1);
	}

	#if _DEBUG
	CLogger::Write("* File contents:");
	#endif

	while(std::getline(fileStream, sLine))
	{
		#if _DEBUG
		CLogger::Write("Line %d: %s", iLineNumber, sLine.c_str());
		#endif

		// Loop through the source, character by character
		for(size_t i = 0; i < sLine.length(); i++)
		{
			// Get the character at the current position
			char cCurrentChar = sLine[i];

			// Current character is a space or a tab
			if(cCurrentChar == ' ' || cCurrentChar == '\t')
			{
				// Make sure we actually have a token before pushing it onto the list
				// An example of when we wouldn't have a token is when parsing the ' = ' part of 'int test = 5;'
				// When it comes to the second space there's no token yet (seeing as the = sign was just pushed onto the list)
				if(sTokenValue.length() > 0)
				{
					AddTokenToList(sTokenValue, iLineNumber);

					// Reset the current token value
					sTokenValue = "";
				}
			}

			// The character is {, }, = or ;
			else if(cCurrentChar == '{' || cCurrentChar == '}' || cCurrentChar == '=' || cCurrentChar == ';')
			{
				// If we already have a token length (eg: when processing '5;'), we first push
				// the already existing token onto the token list before processing the new one we just found
				if(sTokenValue.length() > 0)
				{
					AddTokenToList(sTokenValue, iLineNumber);

					// Reset the current token value
					sTokenValue = "";
				}

				// Set the token string to the one character we just found and push that onto the token list as well
				sTokenValue += cCurrentChar;
				AddTokenToList(sTokenValue, iLineNumber);

				// Reset the current token value
				sTokenValue = "";
			}

			// The character is neither of the above
			else
			{
				// Append the character to the token string
				sTokenValue += cCurrentChar;
			}
		}

		// Increment the line number
		iLineNumber++;
	}

	#if _DEBUG
	CLogger::Write("\n* Tokens found in the source:");

	for(TokenList::iterator iterator = m_lTokenList.begin(); iterator != m_lTokenList.end(); iterator++)
		CLogger::Write("Token found: %s - Value: %s - On line: %d", getStringFromTokenType((*iterator).m_iTokenType), (*iterator).m_sValue.c_str(), (*iterator).m_iLine);
	#endif

}

// Return the token list
std::list<CToken> CTokenizer::GetTokenList()
{
	return m_lTokenList;
}

// This method returns the string type from the token, this method is only available when compiling in debug mode
#if _DEBUG
const char * CTokenizer::getStringFromTokenType(eTokenType eType)
{
	if(eType == CLOSE_BRACKET_TOKEN) return "CLOSE_BRACKET_TOKEN";
	if(eType == OPEN_BRACKET_TOKEN) return "OPEN_BRACKET_TOKEN";
	if(eType == SEMICOLON_TOKEN) return "SEMICOLON_TOKEN";
	if(eType == INTEGER_TYPE_TOKEN) return "INTEGER_TYPE_TOKEN";
	if(eType == FLOAT_TYPE_TOKEN) return "FLOAT_TYPE_TOKEN";
	if(eType == STRING_TYPE_TOKEN) return "STRING_TYPE_TOKEN";
	if(eType == VALUE_TOKEN) return "VALUE_TOKEN";
	if(eType == EQUALSIGN_TOKEN) return "EQUALSIGN_TOKEN";

	return "Invalid token";
}
#endif