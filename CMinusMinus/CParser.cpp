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

void CParser::PushBackError(int iErrorLine, std::string sErrorMessage)
{
	CError oError;
	oError.m_iLine = iErrorLine;
	oError.m_sMessage = sErrorMessage;
	m_lErrorList.push_back(oError);
}

// Returns true if the variable exists on the variable list, false otherwise
bool CParser::VariableExists(std::string sVariableName)
{
	bool bVariableFound = false;

	// Loop through all the variables
	for(VariableList::iterator iterator = m_lVariableList.begin(); iterator != m_lVariableList.end(); iterator++)
	{
		// If we have a name match, set the bVariableFound bool to true
		if((*iterator).m_sValueName == sVariableName)
			bVariableFound = true;
	}

	return bVariableFound;
}

// This method returns true if both CIndentation levels are either the same or oToAccess
// is allowed to access variables on oToBeAccessed
bool CParser::HasCorrectIndentationLevel(CIndentation oToBeAccessed, CIndentation oToAccess)
{
	// If the access level is the same, and the indentation ID is the same, return true
	if(oToBeAccessed.m_iLevel == oToAccess.m_iLevel && oToBeAccessed.m_iLevelID == oToAccess.m_iLevelID)
		return true;

	// If oToBeAccessed's level is smaller than oToAccess, return true
	if(oToBeAccessed.m_iLevel < oToAccess.m_iLevel && oToBeAccessed.m_iLevelID < oToAccess.m_iLevelID)
		return true;

	// We didn't return anything yet, no correct indentation level
	return false;
}

void CParser::Run()
{
	// Loop through the entire token list
	for(TokenList::iterator iterator = m_lTokenList.begin(); iterator != m_lTokenList.end(); iterator++)
	{
		// Get the current token
		CToken CurrentToken = (*iterator);
		// Get the previous token on the list
		CToken PreviousToken = CToken();
		// Get the token before the previous token on the list (used to check in the 'something = somethingelse' kind of checks)
		CToken SecondPreviousToken = CToken();

		// If the current iterator isn't at the beginning of the list, we can get the token before that
		if(iterator != m_lTokenList.begin())
			PreviousToken = (*(--iterator));

		// If the PreviousToken (the token before the CurrentToken) isn't at the start of the list yet, we can get the token before that one
		if(iterator != m_lTokenList.begin())
			SecondPreviousToken = (*(--iterator));	

		// We now have to increment the iterator again, otherwise the loop will run infinitely
		// If the PreviousToken was set to something else than CToken() (eg iterator-- was run)
		// increment the iterator.
		if(PreviousToken.m_iTokenType != INVALID_TOKEN_TYPE)
			iterator++;

		// If the SecondPreviousToken was set to something else than CToken(), iterator-- has run twice
		// Call iterator++ again.
		if(SecondPreviousToken.m_iTokenType != INVALID_TOKEN_TYPE)
			iterator++;

		// Check if the iterator is currently at the start of the list
		// If it is, we need to perform some seperate checks
		if(iterator == m_lTokenList.begin())
		{
			// The only things allowed at the start of the script is a { or type
			if(CurrentToken.m_iTokenType != OPEN_BRACKET_TOKEN && CurrentToken.m_iTokenType != FLOAT_TYPE_TOKEN && CurrentToken.m_iTokenType != INTEGER_TYPE_TOKEN && CurrentToken.m_iTokenType != STRING_TYPE_TOKEN)
				PushBackError(CurrentToken.m_iLine, "Unexpected '" + CurrentToken.m_sValue + "' at start of the script found.");

			// We don't need to execute the rest of the checks, call continue
			continue;
		}

		// If the previous token was an equal sign, and we have a token before that, we're in an assignement statement
		if(PreviousToken.m_iTokenType == EQUALSIGN_TOKEN && SecondPreviousToken.m_iTokenType != INVALID_TOKEN_TYPE)
		{
			// Variable or constant doesn't exist
			if(!VariableExists(SecondPreviousToken.m_sValue))
			{
				// The user is trying to assign something to a constant value (for example: int 5 = 3;)
				if(IsFloatOrInteger(SecondPreviousToken.m_sValue))
					PushBackError(CurrentToken.m_iLine, "Cannot assign to a value constant (" + SecondPreviousToken.m_sValue + ").");

				// The variable isn't a float or integer, it simply doesn't exist
				if(!IsFloatOrInteger(SecondPreviousToken.m_sValue))
					PushBackError(CurrentToken.m_iLine, "Cannot assign to '" + SecondPreviousToken.m_sValue + "', that variable does not exist.");
			}

			// If the current token isn't a float or integer and it doesn't exist, we're either dealing
			// with a string literal or another variable we're assigning to
			if(!IsFloatOrInteger(CurrentToken.m_sValue) && !VariableExists(CurrentToken.m_sValue))
			{
				// Check if it's a string literal
				if(CurrentToken.m_iTokenType == STRING_LITERAL_TOKEN)
				{
					// Remove the leading and trailing double quote
					std::string sStringLiteral = CurrentToken.m_sValue.erase(0, 1);
					sStringLiteral = sStringLiteral.erase((CurrentToken.m_sValue.length() - 1), 1);
					
					// Loop through all the variables, so we can find the iterator that represents the variable we're trying to assign something to
					for(VariableList::iterator iterator = m_lVariableList.begin(); iterator != m_lVariableList.end(); iterator++)
					{
						// Check if the iterator name is equal to the variable name we're trying to assign something to
						if((*iterator).m_sValueName == SecondPreviousToken.m_sValue)
						{
							// Set the value to the string variable
							(*iterator).m_sValue = sStringLiteral;
							(*iterator).m_bHasBeenAssignedAnything = true;
						}
					}
				}

				// It's not a string literal, and the variable doesn't exist, show an error
				else PushBackError(CurrentToken.m_iLine, "Cannot assign '" + CurrentToken.m_sValue + "' to '" + SecondPreviousToken.m_sValue + "', '" + CurrentToken.m_sValue + "' does not exist");	
			}

			// Loop through all the variables, so we can find the iterator that represents the variable we're trying to assign something to
			for(VariableList::iterator iterator = m_lVariableList.begin(); iterator != m_lVariableList.end(); iterator++)
			{
				// Check if the iterator name is equal to the variable name we're trying to assign something to
				if((*iterator).m_sValueName == SecondPreviousToken.m_sValue)
				{	
					// Check if the current token is a value constant
					// We handle 'var = constants' type of statements here
					if(IsFloatOrInteger(CurrentToken.m_sValue))
					{
						// Set the hasBeenAssignedAnything flag for this variable to true
						// This flags the variable as been defined
						(*iterator).m_bHasBeenAssignedAnything = true;

						// Is it an integer constant?
						if(IsInteger(CurrentToken.m_sValue))
						{
							// Add some typechecking
							if((*iterator).m_eType != VARIABLE_TYPE_INTEGER)
								PushBackError(CurrentToken.m_iLine, "Cannot assign '" + CurrentToken.m_sValue + "' to '" + SecondPreviousToken.m_sValue + "', the types differ.");

							// Set the value for this variable
							(*iterator).m_iValue = atoi(CurrentToken.m_sValue.c_str());
						}
						// Or a float constant
						else
						{
							// Add some typechecking
							if((*iterator).m_eType != VARIABLE_TYPE_FLOAT)
								PushBackError(CurrentToken.m_iLine, "Cannot assign '" + CurrentToken.m_sValue + "' to '" + SecondPreviousToken.m_sValue + "', the types differ.");

							// Set the value for this variable
							(*iterator).m_fValue = atof(CurrentToken.m_sValue.c_str());
						}
					}

					// The current token isn't a value constant, it's another variable
					// We handle 'var = var' type of statements here
					else
					{
						// We need to get the value of the variable we're trying to assign the left hand side to
						for(VariableList::iterator secondIterator = m_lVariableList.begin(); secondIterator != m_lVariableList.end(); secondIterator++)
						{
							// Get the variable we want to get the value from
							if((*secondIterator).m_sValueName == CurrentToken.m_sValue)			
							{
								// Check if the variable is allowed the other variable
								if(!HasCorrectIndentationLevel((*secondIterator).m_oIndentation, SecondPreviousToken.m_oIndentation))
								{
									PushBackError(CurrentToken.m_iLine, "Cannot access " + (*secondIterator).m_sValueName + ", that variable is declared on another level.");
									break;
								}

								// Type checking: make sure the variables have the same types
								if((*iterator).m_eType != (*secondIterator).m_eType)
								{
									PushBackError(CurrentToken.m_iLine, "Cannot assign '" + CurrentToken.m_sValue + "' to '" + SecondPreviousToken.m_sValue + "', the types differ.");
									break;
								}

								// Set the hasBeenAssignedAnything flag to true
								// This flags the variable as been defined
								(*iterator).m_bHasBeenAssignedAnything = true;

								// Set the value
								if((*iterator).m_eType == VARIABLE_TYPE_INTEGER)
									(*iterator).m_iValue = (*secondIterator).m_iValue;
								
								if((*iterator).m_eType == VARIABLE_TYPE_FLOAT)
									(*iterator).m_fValue = (*secondIterator).m_fValue;

								if((*iterator).m_eType == VARIABLE_TYPE_STRING)
									(*iterator).m_sValue = (*secondIterator).m_sValue;
							}
						}
					}

					// Once we found the correct variable name, we don't want to check the rest of the variables
					// Break out of the loop
					break;
				}
			}
		}

		// Convert the line number of the previous token to a string
		std::stringstream sLineNumberOfPreviousToken;
		sLineNumberOfPreviousToken << PreviousToken.m_iLine;
		std::string sPreviousTokensLine = sLineNumberOfPreviousToken.str();

		if(CurrentToken.m_iTokenType == OPEN_BRACKET_TOKEN)
		{
			// Allowed previous tokens: {, }, ;
			// Not allowed previous tokens: {, float, string, int, =, VALUE_TOKEN
			if(PreviousToken.m_iTokenType != OPEN_BRACKET_TOKEN && PreviousToken.m_iTokenType != CLOSE_BRACKET_TOKEN && PreviousToken.m_iTokenType != SEMICOLON_TOKEN)
				PushBackError(CurrentToken.m_iLine, "Finish the statement at line " + sPreviousTokensLine + " first.");
		}

		if(CurrentToken.m_iTokenType == CLOSE_BRACKET_TOKEN)
		{
			// Allowed previous tokens: {, }, ;
			// Not allowed previous tokens: }, float, string, int, =, VALUE_TOKEN
			if(PreviousToken.m_iTokenType != OPEN_BRACKET_TOKEN && PreviousToken.m_iTokenType != CLOSE_BRACKET_TOKEN && PreviousToken.m_iTokenType != SEMICOLON_TOKEN)
				PushBackError(CurrentToken.m_iLine, "Finish the statement at line " + sPreviousTokensLine + " first.");
		}

		if(CurrentToken.m_iTokenType == SEMICOLON_TOKEN)
		{
			// Allowed previous tokens: {, ;, }, VALUE_TOKEN
			// Not allowed previous tokens: =, float, string, int
			if(PreviousToken.m_iTokenType != CLOSE_BRACKET_TOKEN && PreviousToken.m_iTokenType != OPEN_BRACKET_TOKEN && PreviousToken.m_iTokenType != VALUE_TOKEN && PreviousToken.m_iTokenType != SEMICOLON_TOKEN)
			{
				if(PreviousToken.m_iTokenType == STRING_TYPE_TOKEN || PreviousToken.m_iTokenType == INTEGER_TYPE_TOKEN || PreviousToken.m_iTokenType == FLOAT_TYPE_TOKEN)
					PushBackError(CurrentToken.m_iLine, "Expected an equal sign followed by a value or variable on line " + sPreviousTokensLine);

				if(PreviousToken.m_iTokenType == EQUALSIGN_TOKEN)
					PushBackError(CurrentToken.m_iLine, "Expected a value or variable after the equal sign on line " + sPreviousTokensLine);
			}
		}

		if(CurrentToken.m_iTokenType == EQUALSIGN_TOKEN)
		{
			// Allowed previous tokens: VALUE_TOKEN
			// Not allowed previous tokens: =, float, string, int, {, ;, }
			if(PreviousToken.m_iTokenType != VALUE_TOKEN)
				PushBackError(CurrentToken.m_iLine, PreviousToken.m_sValue + " cannot be followed by an equal sign.");
		}

		if(CurrentToken.m_iTokenType == INTEGER_TYPE_TOKEN || CurrentToken.m_iTokenType == FLOAT_TYPE_TOKEN || CurrentToken.m_iTokenType == STRING_TYPE_TOKEN)
		{
			// Allowed previous tokens: {, }, ;
			// Not allowed previous tokens: =, float, string, int, VALUE_TOKEN
			if(PreviousToken.m_iTokenType != CLOSE_BRACKET_TOKEN && PreviousToken.m_iTokenType != OPEN_BRACKET_TOKEN && PreviousToken.m_iTokenType != SEMICOLON_TOKEN)
				PushBackError(CurrentToken.m_iLine, PreviousToken.m_sValue + " cannot be followed by a type.");
		}

		if(CurrentToken.m_iTokenType == VALUE_TOKEN)
		{
			// Allowed previous tokens: {, }, ;, =, float, string, int, =
			// Not allowed previous tokens: VALUE_TOKEN
			if(PreviousToken.m_iTokenType == VALUE_TOKEN)
			{
				PushBackError(CurrentToken.m_iLine, "'" + PreviousToken.m_sValue + "' cannot be followed by '" + CurrentToken.m_sValue + "'.");
			}
			else
			{
				// If the current token is a value token
				// and the previous token was either a float, string or int type, the user is trying to declare a variable
				if(PreviousToken.m_iTokenType == FLOAT_TYPE_TOKEN || PreviousToken.m_iTokenType == INTEGER_TYPE_TOKEN || PreviousToken.m_iTokenType == STRING_TYPE_TOKEN)
				{
					// Check if the current token is a valid variable name
					if(!IsFloatOrInteger(CurrentToken.m_sValue))
					{
						if(VariableExists(CurrentToken.m_sValue))
						{
							PushBackError(CurrentToken.m_iLine, "'" + CurrentToken.m_sValue + "' already exists. Cannot re-declare a variable.");
						}
						else
						{
							// Setup a CVariable object
							CVariable oVariable;
							oVariable.m_sValueName = CurrentToken.m_sValue;

							// Set the type of the CVariable object according to the type of token the previous token object had
							if(PreviousToken.m_iTokenType == INTEGER_TYPE_TOKEN)
								oVariable.m_eType = VARIABLE_TYPE_INTEGER;

							if(PreviousToken.m_iTokenType == FLOAT_TYPE_TOKEN)
								oVariable.m_eType = VARIABLE_TYPE_FLOAT;

							if(PreviousToken.m_iTokenType == STRING_TYPE_TOKEN)
								oVariable.m_eType = VARIABLE_TYPE_STRING;

							// Save the indentation level for this variable
							oVariable.m_oIndentation = CurrentToken.m_oIndentation;

							// Push it onto the variable list
							m_lVariableList.push_back(oVariable);
						}
					}
				}
			}
		}
	}

	// Now loop through the error list
	#if _DEBUG
	if(m_lErrorList.size() > 0) CLogger::Write("\n* Errors found:");
	#endif

	// Log every error
	for(ErrorList::iterator iterator = m_lErrorList.begin(); iterator != m_lErrorList.end(); iterator++)
		CLogger::Write("Line %d: %s", (*iterator).m_iLine, (*iterator).m_sMessage.c_str());

	// If we're compiling in debug mode we show the variables we've found in the scripts
	#if _DEBUG
	CLogger::Write("\n* Variables found:");
	for(VariableList::iterator iterator = m_lVariableList.begin(); iterator != m_lVariableList.end(); iterator++)
	{
		// Check if the variable has been assigned anything
		if((*iterator).m_bHasBeenAssignedAnything)
		{
			// Output the variable name and type
			if((*iterator).m_eType == VARIABLE_TYPE_INTEGER)
				CLogger::Write("Variable %s (integer) has value %d (tab level: %d, tab id: %d)", (*iterator).m_sValueName.c_str(), (*iterator).m_iValue, (*iterator).m_oIndentation.m_iLevel, (*iterator).m_oIndentation.m_iLevelID);

			if((*iterator).m_eType == VARIABLE_TYPE_FLOAT)
				CLogger::Write("Variable %s (float) has value %.2f (tab level: %d, tab id: %d)", (*iterator).m_sValueName.c_str(), (*iterator).m_fValue, (*iterator).m_oIndentation.m_iLevel, (*iterator).m_oIndentation.m_iLevelID);

			if((*iterator).m_eType == VARIABLE_TYPE_STRING)
				CLogger::Write("Variable %s (string) has value %s (tab level: %d, tab id: %d)", (*iterator).m_sValueName.c_str(), (*iterator).m_sValue.c_str(), (*iterator).m_oIndentation.m_iLevel, (*iterator).m_oIndentation.m_iLevelID);
		}

		else CLogger::Write("Variable %s has been declared but not yet defined. (tab level: %d, tab id: %d)", (*iterator).m_sValueName.c_str(), (*iterator).m_oIndentation.m_iLevel, (*iterator).m_oIndentation.m_iLevelID);
	}
	#endif
}