//==============================================================================
//
// File: CParser.h
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
// 
// The CParser class checks for any compile errors in the token list.
// 
//==============================================================================

#include "CParser.h"
#include "CLogger.h"
#include "Util.h"
#include <sstream>

CParser::CParser(TokenList lTokenList)
{
	m_lTokenList = lTokenList;
}

void CParser::Run()
{
	// Loop through the entire token list
	for(TokenList::iterator iterator = m_lTokenList.begin(); iterator != m_lTokenList.end(); iterator++)
	{
		// Get the current token
		CToken CurrentToken = (*iterator);

		// Setup the CError object
		CError Error;
		Error.m_iLine = CurrentToken.m_iLine;

		// The iterator is currently not at the beginning of the list
		// This has to be in a seperate check since we can't get the previous token if we're at the start of the list
		if(iterator == m_lTokenList.begin())
		{
			if(CurrentToken.m_iTokenType != OPEN_BRACKET_TOKEN && CurrentToken.m_iTokenType != FLOAT_TYPE_TOKEN && CurrentToken.m_iTokenType != INTEGER_TYPE_TOKEN && CurrentToken.m_iTokenType != STRING_TYPE_TOKEN)
			{
				Error.m_sMessage = "Unexpected '" + CurrentToken.m_sValue + "' at start of the script found.";
			}
		}

		// The iterator is at the beginning of the list
		else
		{
			// Get the previous token on the list
			CToken PreviousToken = (*(--iterator));

			// Convert the line number of the previous token to a string
			std::stringstream sLineNumberOfPreviousToken;
			sLineNumberOfPreviousToken << PreviousToken.m_iLine;
			std::string sPreviousTokensLine = sLineNumberOfPreviousToken.str();

			// We have to increment the iterator again, otherwise this loop will run infinitely
			iterator++;

			if(CurrentToken.m_iTokenType == OPEN_BRACKET_TOKEN)
			{
				// Allowed: {, }, ;
				// Not allowed: {, float, string, int, =, VALUE_TOKEN
				if(PreviousToken.m_iTokenType != OPEN_BRACKET_TOKEN && PreviousToken.m_iTokenType != CLOSE_BRACKET_TOKEN && PreviousToken.m_iTokenType != SEMICOLON_TOKEN)
				{
					Error.m_sMessage = "Finish the statement at line " + sPreviousTokensLine + " first.";
				}
			}

			if(CurrentToken.m_iTokenType == CLOSE_BRACKET_TOKEN)
			{
				// Allowed: {, }, ;
				// Not allowed: }, float, string, int, =, VALUE_TOKEN
				if(PreviousToken.m_iTokenType != OPEN_BRACKET_TOKEN && PreviousToken.m_iTokenType != CLOSE_BRACKET_TOKEN && PreviousToken.m_iTokenType != SEMICOLON_TOKEN)
				{
					Error.m_sMessage = "Finish the statement at line " + sPreviousTokensLine + " first.";
				}
			}

			if(CurrentToken.m_iTokenType == SEMICOLON_TOKEN)
			{
				// Allowed: {, ;, }, VALUE_TOKEN
				// Not allowed: =, float, string, int
				if(PreviousToken.m_iTokenType != CLOSE_BRACKET_TOKEN && PreviousToken.m_iTokenType != OPEN_BRACKET_TOKEN && PreviousToken.m_iTokenType != VALUE_TOKEN && PreviousToken.m_iTokenType != SEMICOLON_TOKEN)
				{
					if(PreviousToken.m_iTokenType == STRING_TYPE_TOKEN || PreviousToken.m_iTokenType == INTEGER_TYPE_TOKEN || PreviousToken.m_iTokenType == FLOAT_TYPE_TOKEN)
					{
						Error.m_sMessage = "Expected an equal sign followed by a statement on line " + sPreviousTokensLine;
					}

					if(PreviousToken.m_iTokenType == EQUALSIGN_TOKEN)
					{
						Error.m_sMessage = "Expected a statement after the equal sign on line " + sPreviousTokensLine;
					}
				}
			}

			if(CurrentToken.m_iTokenType == EQUALSIGN_TOKEN)
			{
				// Allowed: VALUE_TOKEN
				// Not allowed: =, float, string, int, {, ;, }
				if(PreviousToken.m_iTokenType != VALUE_TOKEN)
				{
					Error.m_sMessage = PreviousToken.m_sValue + " cannot be followed by an equal sign.";
				}
			}

			if(CurrentToken.m_iTokenType == INTEGER_TYPE_TOKEN || CurrentToken.m_iTokenType == FLOAT_TYPE_TOKEN || CurrentToken.m_iTokenType == STRING_TYPE_TOKEN)
			{
				// Allowed: {, }, ;
				// Not allowed: =, float, string, int, VALUE_TOKEN
				if(PreviousToken.m_iTokenType != CLOSE_BRACKET_TOKEN && PreviousToken.m_iTokenType != OPEN_BRACKET_TOKEN && PreviousToken.m_iTokenType != SEMICOLON_TOKEN)
				{
					Error.m_sMessage = PreviousToken.m_sValue + " cannot be followed by a type.";
				}
			}

			if(CurrentToken.m_iTokenType == VALUE_TOKEN)
			{
				// Allowed: {, }, ;, =, float, string, int, =
				// Not allowed: VALUE_TOKEN
				if(PreviousToken.m_iTokenType == VALUE_TOKEN)
				{
					Error.m_sMessage = PreviousToken.m_sValue + " cannot be followed by '" + CurrentToken.m_sValue + "'.";
				}
			}
		}

		// If we have an error, push it back
		if(Error.m_sMessage != "")
			m_lErrorList.push_back(Error);
	}

	// Now loop through the error list
	#if _DEBUG
	if(m_lErrorList.size() > 0) CLogger::Write("\n* Errors found:");
	#endif

	for(ErrorList::iterator iterator = m_lErrorList.begin(); iterator != m_lErrorList.end(); iterator++)
	{
		CLogger::Write("Line %d: %s", (*iterator).m_iLine, (*iterator).m_sMessage.c_str());
	}
}