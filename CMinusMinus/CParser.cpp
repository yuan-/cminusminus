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
#include "CFunctionWrapper.h"

#include <sstream>

// Constructor of the CParser class
CParser::CParser(TokenList lTokenList)
{
	m_lTokenList = lTokenList;
}

// Pushes back an error onto the error list
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
	// Loop through all the variables
	for(VariableList::iterator iterator = m_lVariableList.begin(); iterator != m_lVariableList.end(); iterator++)
	{
		// If we have a name match, set the bVariableFound bool to true
		if((*iterator).m_sValueName == sVariableName)
			return true;
	}

	return false;
}

// This method returns a token list iterator from a variable name
VariableList::iterator CParser::GetVariableListIteratorFromVariableName(std::string sVariableName)
{
	// Loop through all the variables, so we can find the iterator that represents the variable we're trying to assign something to
	for(VariableList::iterator iterator = m_lVariableList.begin(); iterator != m_lVariableList.end(); iterator++)
	{
		// Check if the iterator name is equal to the variable name we're trying to assign something to
		if((*iterator).m_sValueName == sVariableName)
			return iterator;
	}

	return m_lVariableList.end();
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

// Returns the variable type from a string value
// For example: "3" returns integer, "3.14" float and "Hello" string
eVariableTypes CParser::GetVariableType(std::string sValue)
{
	// Either an integer or float
	if(IsFloatOrInteger(sValue))
	{
		// Integer
		if(IsInteger(sValue))
			return VARIABLE_TYPE_INTEGER;

		// Float
		return VARIABLE_TYPE_FLOAT;
	}
	// String
	else return VARIABLE_TYPE_STRING;
}

void CParser::Run()
{
	// Loop through the entire token list
	for(size_t i = 0; i < m_lTokenList.size(); i++)
	{
		// Get the current token
		CToken CurrentToken = m_lTokenList[i];
		// Get the previous token on the list
		CToken PreviousToken = (i > 0) ? m_lTokenList[i - 1] : CToken();
		// Get the token before the previous token on the list (used to check in the 'something = somethingelse' kind of checks)
		CToken SecondPreviousToken = (i > 1) ? m_lTokenList[i - 2] : CToken();

		// Check if the iterator is currently at the start of the list
		// If it is, we need to perform some seperate checks
		if(i == 0)
		{
			// The only things allowed at the start of the script is a { or type
			if(CurrentToken.m_iTokenType != OPEN_CURLY_BRACKET_TOKEN && CurrentToken.m_iTokenType != FLOAT_TYPE_TOKEN && CurrentToken.m_iTokenType != INTEGER_TYPE_TOKEN && CurrentToken.m_iTokenType != STRING_TYPE_TOKEN)
				PushBackError(CurrentToken.m_iLine, "Unexpected '" + CurrentToken.m_sValue + "' at start of the script found.");

			// We don't need to execute the rest of the checks, call continue
			continue;
		}

		// If the previous token was an equal sign, and we have a token before that, we're in an assignement statement
		if(PreviousToken.m_iTokenType == EQUALSIGN_TOKEN && SecondPreviousToken.m_iTokenType != INVALID_TOKEN_TYPE)
		{
			// First check if we're assigning to anything valid
			// It cannot be a value constant, string literal or non-existing variable
			if(!VariableExists(SecondPreviousToken.m_sValue))
			{
				// The user is trying to assign something to a constant value (for example: int 5 = 3;)
				if(IsFloatOrInteger(SecondPreviousToken.m_sValue))
					PushBackError(CurrentToken.m_iLine, "Cannot assign to a value constant (" + SecondPreviousToken.m_sValue + ").");

				// It's a string literal
				else if(SecondPreviousToken.m_iTokenType == STRING_LITERAL_TOKEN)
					PushBackError(CurrentToken.m_iLine, "Cannot assign anything to a string literal.");

				// Variable simply doesn't exist
				else
					PushBackError(CurrentToken.m_iLine, "Cannot assign anything to " + SecondPreviousToken.m_sValue + ", variable does not exist.");
				continue;
			}

			// Get the iterator in the VariableList that represents the variable we're assigning to
			VariableList::iterator LeftHandSide = GetVariableListIteratorFromVariableName(SecondPreviousToken.m_sValue);

			// Now check if we're trying to assign something valid to the variable
			if(!VariableExists(CurrentToken.m_sValue))
			{
				// Check if the current token is a value constant
				// We handle 'var = constants' type of statements here

				// Check if it's a string literal
				if(CurrentToken.m_iTokenType == STRING_LITERAL_TOKEN)
				{
					// Make sure we're assigning it to a string
					if((*LeftHandSide).m_eType != VARIABLE_TYPE_STRING)
					{
						PushBackError(CurrentToken.m_iLine, "Cannot assign \"" + CurrentToken.m_sValue + "\" to '" + SecondPreviousToken.m_sValue + "', the types differ.");
						continue;
					}

					// Set the hasBeenAssignedAnything flag for this variable to true
					// This flags the variable as been defined
					(*LeftHandSide).m_sValue = CurrentToken.m_sValue;

					// Set the value for this variable
					(*LeftHandSide).m_bHasBeenAssignedAnything = true;
				}

				// It's a float or integer
				else if(IsFloatOrInteger(CurrentToken.m_sValue))
				{
					// Is it an integer constant?
					if(IsInteger(CurrentToken.m_sValue))
					{
						// Type checking
						if((*LeftHandSide).m_eType != VARIABLE_TYPE_INTEGER)
						{
							PushBackError(CurrentToken.m_iLine, "Cannot assign '" + CurrentToken.m_sValue + "' to '" + SecondPreviousToken.m_sValue + "', the types differ.");
							continue;
						}

						// Set the hasBeenAssignedAnything flag for this variable to true
						// This flags the variable as been defined
						(*LeftHandSide).m_bHasBeenAssignedAnything = true;

						// Set the value for this variable
						(*LeftHandSide).m_iValue = atoi(CurrentToken.m_sValue.c_str());
					}

					// Or a float constant
					else
					{
						// Add some typechecking
						if((*LeftHandSide).m_eType != PARAMETER_TYPE_FLOAT)
						{
							PushBackError(CurrentToken.m_iLine, "Cannot assign '" + CurrentToken.m_sValue + "' to '" + SecondPreviousToken.m_sValue + "', the types differ.");
							continue;
						}

						// Set the hasBeenAssignedAnything flag for this variable to true
						// This flags the variable as been defined
						(*LeftHandSide).m_bHasBeenAssignedAnything = true;

						// Set the value for this variable
						(*LeftHandSide).m_fValue = atof(CurrentToken.m_sValue.c_str());
					}
				}

				// It's not a string literal, not a float or integer, this means the user is trying to assign a variable to another variable
				// While the right hand side variable doesn't exist
				else
				{
					// Wait, it might be a function call, make sure the next token isn't a OPEN_BRACKET_TOKEN
					if((i + 1) != m_lTokenList.size() && m_lTokenList[i + 1].m_iTokenType != OPEN_BRACKET_TOKEN)
					{
						// It wasn't, the rhs doesn't exist
						PushBackError(CurrentToken.m_iLine, "Cannot assign '" + CurrentToken.m_sValue + "' to '" + SecondPreviousToken.m_sValue + "', '" + CurrentToken.m_sValue + "' does not exist.");
						continue;
					}
				}
			}

			// The current token isn't a value constant, it's another variable
			// We handle 'var = var' type of statements here
			else
			{
				// Get the iterator for the right hand side variable
				VariableList::iterator RightHandSide = GetVariableListIteratorFromVariableName(CurrentToken.m_sValue);

				// Check if the variable is allowed the other variable
				if(!HasCorrectIndentationLevel((*RightHandSide).m_oIndentation, SecondPreviousToken.m_oIndentation))
				{
					PushBackError(CurrentToken.m_iLine, "Cannot access " + (*RightHandSide).m_sValueName + ", that variable is declared on another level.");
					continue;
				}

				// Type checking: make sure the variables have the same types
				if((*LeftHandSide).m_eType != (*RightHandSide).m_eType)
				{
					PushBackError(CurrentToken.m_iLine, "Cannot assign '" + CurrentToken.m_sValue + "' to '" + SecondPreviousToken.m_sValue + "', the types differ.");
					continue;
				}

				// Set the hasBeenAssignedAnything flag to true
				// This flags the variable as been defined
				(*LeftHandSide).m_bHasBeenAssignedAnything = true;

				// Set the value
				if((*LeftHandSide).m_eType == VARIABLE_TYPE_INTEGER)
					(*LeftHandSide).m_iValue = (*RightHandSide).m_iValue;

				if((*LeftHandSide).m_eType == PARAMETER_TYPE_FLOAT)
					(*LeftHandSide).m_fValue = (*RightHandSide).m_fValue;

				if((*LeftHandSide).m_eType == VARIABLE_TYPE_STRING)
					(*LeftHandSide).m_sValue = (*RightHandSide).m_sValue;
			}
		}

		// The previous token was either a + or -
		if(PreviousToken.m_iTokenType == PLUS_OPERATOR_TOKEN || PreviousToken.m_iTokenType == MINUS_OPERATOR_TOKEN)
		{
			// Get the token for the assignment variable (the variable we're assigning to)
			CToken VariableWhichIsBeingAssignedTo = CToken();

			// This variable holds the amounts of times we had to decrement the iterator in order
			// to find the variable we're assigning to. We need to save the amount of times we've
			// decremented the iterator in order to increment it the same amount of times again,
			// otherwise this loop will run infinitely
			int iAmountOfDecrements = i;

			// Infinite loop
			while(true)
			{
				// Wait, if the iterator is already at the start of the list, break out of the loop
				// We can't decrement an iterator which is already at the start of the loop
				if(iAmountOfDecrements == m_lTokenList.size())
					break;

				// Save the current iterator position
				VariableWhichIsBeingAssignedTo = m_lTokenList[iAmountOfDecrements--];

				// If we the token we're processing is a VALUE_TOKEN and it's not a constant value (eg not a float or int)
				if(VariableWhichIsBeingAssignedTo.m_iTokenType == VALUE_TOKEN && !IsFloatOrInteger(VariableWhichIsBeingAssignedTo.m_sValue))
					break;
			}

			// Now we get the VariableList iterator which is pointing at the correct variable we want to assign to
			VariableList::iterator LeftHandSide = GetVariableListIteratorFromVariableName(VariableWhichIsBeingAssignedTo.m_sValue);
			// Get the variable type of the current token (eg what we're trying to assign to our variable)
			eVariableTypes eType = GetVariableType(CurrentToken.m_sValue);

			// Make sure the iterator is correct (it's not correct if the example we're trying to assign to doesn't exist)
			if(LeftHandSide != m_lVariableList.end())
			{
				// Wait, is the type of what we're trying to assign to the variable the same as the variable?
				if(eType != (*LeftHandSide).m_eType)
				{
					PushBackError(CurrentToken.m_iLine, "Cannot concatenate '" + CurrentToken.m_sValue + "' and '" + (*LeftHandSide).m_sValueName + "', the types differ.");
					continue;
				}
				
				// Is this the plus operator?
				if(PreviousToken.m_iTokenType == PLUS_OPERATOR_TOKEN)
				{
					// int + int
					if(eType == VARIABLE_TYPE_INTEGER)
						(*LeftHandSide).m_iValue += atoi(CurrentToken.m_sValue.c_str());
					// float + float
					if(eType == PARAMETER_TYPE_FLOAT)
						(*LeftHandSide).m_fValue += atof(CurrentToken.m_sValue.c_str());
					// string + string
					if(eType == VARIABLE_TYPE_STRING)
					{
						// Remove the double quotes from the string
						std::string sStringLiteral = CurrentToken.m_sValue;

						// Concat the strings
						(*LeftHandSide).m_sValue += sStringLiteral;
					}
				}

				// Is this the minus operator?
				if(PreviousToken.m_iTokenType == MINUS_OPERATOR_TOKEN)
				{
					// int - int
					if(eType == VARIABLE_TYPE_INTEGER)
						(*LeftHandSide).m_iValue -= atoi(CurrentToken.m_sValue.c_str());
					// float - float
					if(eType == PARAMETER_TYPE_FLOAT)
						(*LeftHandSide).m_fValue -= atof(CurrentToken.m_sValue.c_str());
					// String doesn't support operator-
					if(eType == VARIABLE_TYPE_STRING)
						PushBackError(CurrentToken.m_iLine, "The string type does not define the minus operator.");
				}
			}
			continue;
		}

		// Convert the line number of the previous token to a string
		std::stringstream sLineNumberOfPreviousToken;
		sLineNumberOfPreviousToken << PreviousToken.m_iLine;
		std::string sPreviousTokensLine = sLineNumberOfPreviousToken.str();

		if(CurrentToken.m_iTokenType == OPEN_CURLY_BRACKET_TOKEN)
		{
			// Allowed previous tokens: {, }, ;
			// Not allowed previous tokens: {, float, string, int, =, VALUE_TOKEN
			if(PreviousToken.m_iTokenType != OPEN_CURLY_BRACKET_TOKEN && PreviousToken.m_iTokenType != CLOSE_CURLY_BRACKET_TOKEN && PreviousToken.m_iTokenType != SEMICOLON_TOKEN)
				PushBackError(CurrentToken.m_iLine, "Finish the statement at line " + sPreviousTokensLine + " first.");
			
			continue;
		}

		if(CurrentToken.m_iTokenType == CLOSE_CURLY_BRACKET_TOKEN)
		{
			// Allowed previous tokens: {, }, ;
			// Not allowed previous tokens: }, float, string, int, =, VALUE_TOKEN
			if(PreviousToken.m_iTokenType != OPEN_CURLY_BRACKET_TOKEN && PreviousToken.m_iTokenType != CLOSE_CURLY_BRACKET_TOKEN && PreviousToken.m_iTokenType != SEMICOLON_TOKEN)
				PushBackError(CurrentToken.m_iLine, "Finish the statement at line " + sPreviousTokensLine + " first.");

			continue;
		}

		if(CurrentToken.m_iTokenType == SEMICOLON_TOKEN)
		{
			// Allowed previous tokens: {, ;, }, VALUE_TOKEN
			// Not allowed previous tokens: =, float, string, int
			if(PreviousToken.m_iTokenType != CLOSE_CURLY_BRACKET_TOKEN && PreviousToken.m_iTokenType != OPEN_CURLY_BRACKET_TOKEN && PreviousToken.m_iTokenType != VALUE_TOKEN && PreviousToken.m_iTokenType != SEMICOLON_TOKEN)
			{
				if(PreviousToken.m_iTokenType == STRING_TYPE_TOKEN || PreviousToken.m_iTokenType == INTEGER_TYPE_TOKEN || PreviousToken.m_iTokenType == FLOAT_TYPE_TOKEN)
					PushBackError(CurrentToken.m_iLine, "Expected an equal sign followed by a value or variable on line " + sPreviousTokensLine);

				if(PreviousToken.m_iTokenType == EQUALSIGN_TOKEN)
					PushBackError(CurrentToken.m_iLine, "Expected a value or variable after the equal sign on line " + sPreviousTokensLine);
			}

			continue;
		}

		if(CurrentToken.m_iTokenType == EQUALSIGN_TOKEN)
		{
			// Allowed previous tokens: VALUE_TOKEN
			// Not allowed previous tokens: =, float, string, int, {, ;, }
			if(PreviousToken.m_iTokenType != VALUE_TOKEN)
				PushBackError(CurrentToken.m_iLine, PreviousToken.m_sValue + " cannot be followed by an equal sign.");

			continue;
		}

		if(CurrentToken.m_iTokenType == INTEGER_TYPE_TOKEN || CurrentToken.m_iTokenType == FLOAT_TYPE_TOKEN || CurrentToken.m_iTokenType == STRING_TYPE_TOKEN)
		{
			// Allowed previous tokens: {, }, ;
			// Not allowed previous tokens: =, float, string, int, VALUE_TOKEN
			if(PreviousToken.m_iTokenType != CLOSE_CURLY_BRACKET_TOKEN && PreviousToken.m_iTokenType != OPEN_CURLY_BRACKET_TOKEN && PreviousToken.m_iTokenType != SEMICOLON_TOKEN)
				PushBackError(CurrentToken.m_iLine, PreviousToken.m_sValue + " cannot be followed by a type.");

			continue;
		}

		if(CurrentToken.m_iTokenType == VALUE_TOKEN)
		{
			// If the current token is a value token and the next one is an open bracket token, the user is trying to call a function
			if(i != m_lTokenList.size() && m_lTokenList[i + 1].m_iTokenType == OPEN_BRACKET_TOKEN)
			{
				// Get the function name
				std::string FunctionName = CurrentToken.m_sValue;

				// The LoopToken is the token we'll be using in the loop.
				CToken LoopToken = m_lTokenList[i + 1];
				// We also save the token before the current token, this is used to get a parameter list
				CToken TokenBeforeCurrentLoopToken = m_lTokenList[i];

				// This variable holds the amounts of times we've executed the loop
				int iAmountOfIncrements = i;
				// The parameter list for the function
				ParameterList lParameterList;

				// Infinite loop
				while(true)
				{
					// If the iAmountOfIncrements variable is equal to the token size, we can't get the next token on the list
					// Break out of the loop
					if(iAmountOfIncrements == m_lTokenList.size())
						break;

					// We found a close bracket token, exit the loop
					if(LoopToken.m_iTokenType == CLOSE_BRACKET_TOKEN)
						break;

					// The current token is a value token or a string literal, and the previous token was either a ( or a ,
					// This is a parameter for the function, push it back onto the list
					if(LoopToken.m_iTokenType == VALUE_TOKEN || LoopToken.m_iTokenType == STRING_LITERAL_TOKEN && (TokenBeforeCurrentLoopToken.m_iTokenType == OPEN_BRACKET_TOKEN || TokenBeforeCurrentLoopToken.m_iTokenType == COMMA_TOKEN))
					{
						// It might be a variable, does it exist?
						if(!VariableExists(LoopToken.m_sValue))
						{
							// Doesn't exist
							// It's a float or integer
							if(IsFloatOrInteger(LoopToken.m_sValue))
							{
								// It's an integer
								if(IsInteger(LoopToken.m_sValue))
									lParameterList.push_back(CParameter(PARAMETER_TYPE_INTEGER, atoi(LoopToken.m_sValue.c_str())));
								// It's a float
								else
									lParameterList.push_back(CParameter(PARAMETER_TYPE_FLOAT, (float) atof(LoopToken.m_sValue.c_str())));
							}
							// It's not a float or integer, must be a string
							else lParameterList.push_back(CParameter(PARAMETER_TYPE_STRING, LoopToken.m_sValue));
						}

						// It's a variable, this parameter was a variable
						else
						{
							// Get the iterator on the VariableList
							VariableList::iterator variableIterator = GetVariableListIteratorFromVariableName(LoopToken.m_sValue);

							// Get the type of the variable and push it back onto the parameter list
							if((*variableIterator).m_eType == VARIABLE_TYPE_INTEGER)
								lParameterList.push_back(CParameter(PARAMETER_TYPE_INTEGER, (*variableIterator).m_iValue));
							if((*variableIterator).m_eType == VARIABLE_TYPE_FLOAT)
								lParameterList.push_back(CParameter(PARAMETER_TYPE_FLOAT, (float) (*variableIterator).m_fValue));
							if((*variableIterator).m_eType == VARIABLE_TYPE_STRING)
								lParameterList.push_back(CParameter(PARAMETER_TYPE_STRING, (*variableIterator).m_sValue));
						}
					}

					// Save the current token and the token before the previous one
					TokenBeforeCurrentLoopToken = m_lTokenList[iAmountOfIncrements];
					LoopToken = m_lTokenList[++iAmountOfIncrements];
				}

				// Wait, does the function exist?
				if(!CFunctionWrapper::FunctionExists(FunctionName))
				{
					PushBackError(CurrentToken.m_iLine, "Could not call " + FunctionName + ", function does not exist.");
					continue;
				}

				// Call the function
				CFunctionCallAttempt oAttempt = CFunctionWrapper::CallFunction(FunctionName, lParameterList);

				// Did an error occur while calling the function?
				if(oAttempt.m_bErrorOccured)
				{
					PushBackError(CurrentToken.m_iLine, oAttempt.m_sErrorMessage);
					continue;
				}

				// If the token before the function name token is an equal sign token, this isn't a regular call
				// It's an assignment statement.
				if(PreviousToken.m_iTokenType == EQUALSIGN_TOKEN)
				{
					// Get the variable iterator pointing to the variable we're trying to assign to
					VariableList::iterator VariableAssignmentIterator = GetVariableListIteratorFromVariableName(SecondPreviousToken.m_sValue);

					// Does the return type of the function match the variable's type?
					if((*VariableAssignmentIterator).m_eType != oAttempt.m_oReturnValue.m_eType)
					{
						PushBackError(CurrentToken.m_iLine, "Could not assign the return value of " + FunctionName + " to " + (*VariableAssignmentIterator).m_sValueName + ", the types differ.");
						continue;
					}

					// Something has been assigned to this value
					(*VariableAssignmentIterator).m_bHasBeenAssignedAnything = true;

					// Save the value
					if(oAttempt.m_oReturnValue.m_eType == VARIABLE_TYPE_INTEGER)
						(*VariableAssignmentIterator).m_iValue = oAttempt.m_oReturnValue.m_iValue;

					if(oAttempt.m_oReturnValue.m_eType == VARIABLE_TYPE_FLOAT)
						(*VariableAssignmentIterator).m_fValue = oAttempt.m_oReturnValue.m_fValue;

					if(oAttempt.m_oReturnValue.m_eType == VARIABLE_TYPE_STRING)
						(*VariableAssignmentIterator).m_sValue = oAttempt.m_oReturnValue.m_sValue;
				}
			}
			// End of function checking

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

			continue;
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

			if((*iterator).m_eType == PARAMETER_TYPE_FLOAT)
				CLogger::Write("Variable %s (float) has value %.2f (tab level: %d, tab id: %d)", (*iterator).m_sValueName.c_str(), (*iterator).m_fValue, (*iterator).m_oIndentation.m_iLevel, (*iterator).m_oIndentation.m_iLevelID);

			if((*iterator).m_eType == VARIABLE_TYPE_STRING)
				CLogger::Write("Variable %s (string) has value %s (tab level: %d, tab id: %d)", (*iterator).m_sValueName.c_str(), (*iterator).m_sValue.c_str(), (*iterator).m_oIndentation.m_iLevel, (*iterator).m_oIndentation.m_iLevelID);
		}

		else CLogger::Write("Variable %s has been declared but not yet defined. (tab level: %d, tab id: %d)", (*iterator).m_sValueName.c_str(), (*iterator).m_oIndentation.m_iLevel, (*iterator).m_oIndentation.m_iLevelID);
	}
	#endif
}