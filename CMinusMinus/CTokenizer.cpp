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
#include "CIndentation.h"

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
	if(sTokenValue == "\"")
		return DOUBLE_QUOTE_TOKEN;
	if(sTokenValue == "+")
		return PLUS_OPERATOR_TOKEN;
	if(sTokenValue == "-")
		return MINUS_OPERATOR_TOKEN;

	// No valid token found, this must be a function or variable name
	return VALUE_TOKEN;
}

// This method pushes a new token onto the token list
void CTokenizer::AddTokenToList(std::string sTokenValue, int iLineNumber, CIndentation oIndentation)
{
	// Create a new CToken object
	CToken oToken;
	oToken.m_iTokenType = this->GetTokenType(sTokenValue);
	oToken.m_oIndentation = oIndentation;
	oToken.m_iLine = iLineNumber;
	oToken.m_sValue = sTokenValue;

	// Push the object back on the list
	m_lTokenList.push_back(oToken);
}

void CTokenizer::AddStringLiteralToList(std::string sValue, int iLineNumber, CIndentation oIndentation)
{
	// Create a new CToken object
	CToken oToken;
	oToken.m_iTokenType = STRING_LITERAL_TOKEN;
	oToken.m_oIndentation = oIndentation;
	oToken.m_iLine = iLineNumber;
	oToken.m_sValue = sValue;

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
	// This bool is set to true if we're parsing the contents of a string literal
	bool bInStringLiteral = false;
	// This bool is set to true if we're parsing a multi line comment
	bool bInMultiLineComment = false;

	// This variable holds the indentation level the current variable is on
	// Example of a variable on level 0:
	// int test;
	// Example of a variable on level 1:
	// { int test; }
	int iIndentationLevel = 0;

	// Holds the indentation level ID the variable is on. 
	// Two variables can be on the same levels but not in the same enclosing brackets
	// Example: { int test = 42; } { int bla = test; }, bla shouldn't be able to access test
	// Therefore this variable contains a unique ID for each indentation level
	int iIndentationLevelID = 0;

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

	// Loop through the source, one line at a time
	while(std::getline(fileStream, sLine))
	{
		#if _DEBUG
		CLogger::Write("Line %d: %s", iLineNumber, sLine.c_str());
		#endif

		// Loop through the line, one character at a time
		for(size_t i = 0; i < sLine.length(); i++)
		{
			// Get the character at the current position
			char cCurrentChar = sLine[i];

			// Are we in a string literal?
			if(bInStringLiteral)
			{
				// If we've found another ", the user is exiting the string literal parsing
				if(cCurrentChar == '"')
				{
					AddStringLiteralToList("\"" + sTokenValue + "\"", iLineNumber, CIndentation(iIndentationLevel, iIndentationLevelID));

					// Reset the current token value
					sTokenValue = "";

					// We're no longer parsing a string literal
					bInStringLiteral = false;
					continue;
				}

				// As long as we're in the string literal, push the character back onto the list
				sTokenValue += cCurrentChar;
				continue;
			}

			// Are we in a multi line comment?
			if(bInMultiLineComment)
			{
				// End of multi-line comment (/* blah */)
				if(cCurrentChar == '/' && i != 0 && sLine[i - 1] == '*')
				{
					// Reset the token value
					sTokenValue = "";

					// Set the bool to false
					bInMultiLineComment = false;
				}

				// Continue onto the next token
				continue;
			}

			// Current character is a space or a tab
			if(cCurrentChar == ' ' || cCurrentChar == '\t')
			{
				// Make sure we actually have a token before pushing it onto the list
				// An example of when we wouldn't have a token is when parsing the ' = ' part of 'int test = 5;'
				// When it comes to the second space there's no token yet (seeing as the = sign was just pushed onto the list)
				if(sTokenValue.length() > 0)
				{
					AddTokenToList(sTokenValue, iLineNumber, CIndentation(iIndentationLevel, iIndentationLevelID));

					// Reset the current token value
					sTokenValue = "";
				}
			}

			// If we found a double quote, we're entering a string literal
			else if(cCurrentChar == '"')
			{
				// Set the string literal bool to true
				bInStringLiteral = true;
				continue;
			}

			// This is a single line comment
			else if(cCurrentChar == '/' && i != 0 && sLine[i - 1] == '/')
			{
				// Reset the token value
				sTokenValue = "";

				// Break out of the current line parsing
				break;
			}

			// Start of multi-line comment (/* example of multi line comment */)
			else if(cCurrentChar == '*' && i != 0 && sLine[i - 1] == '/')
			{
				// Set the multi line comment bool to true
				bInMultiLineComment = true;

				// Continue onto the next token
				continue;
			}

			else if(cCurrentChar == '{' || cCurrentChar == '}' || cCurrentChar == '=' || cCurrentChar == ';' || cCurrentChar == '+' || cCurrentChar == '-')
			{
				if(cCurrentChar == '{')
				{
					// We found a {, increase the indentation level and the unique indentation id
					iIndentationLevel++;
					iIndentationLevelID++;
				}

				if(cCurrentChar == '}')
				{
					// We found a }, decrease the indentation level, keep increasing the unique id
					iIndentationLevel--;
					iIndentationLevelID++;
				}

				// If we already have a token length (eg: when processing '5;'), we first push
				// the already existing token onto the token list before processing the new one we just found
				if(sTokenValue.length() > 0)
				{
					AddTokenToList(sTokenValue, iLineNumber, CIndentation(iIndentationLevel, iIndentationLevelID));

					// Reset the current token value
					sTokenValue = "";
				}

				// Set the token string to the one character we just found and push that onto the token list as well
				sTokenValue += cCurrentChar;
				AddTokenToList(sTokenValue, iLineNumber, CIndentation(iIndentationLevel, iIndentationLevelID));

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
		CLogger::Write("%s: value: %s, on line: %d", getStringFromTokenType((*iterator).m_iTokenType), (*iterator).m_sValue.c_str(), (*iterator).m_iLine);
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
	if(eType == DOUBLE_QUOTE_TOKEN) return "DOUBLE_QUOTE_TOKEN";
	if(eType == STRING_LITERAL_TOKEN) return "STRING_LITERAL_TOKEN";
	if(eType == PLUS_OPERATOR_TOKEN) return "PLUS_OPERATOR_TOKEN";
	if(eType == MINUS_OPERATOR_TOKEN) return "MINUS_OPERATOR_TOKEN";

	return "Invalid token";
}
#endif