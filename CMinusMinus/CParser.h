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

#include <list>
#include "CToken.h"
#include "CError.h"
#include "CVariable.h"

class CParser
{
	// The value list (this means, variable or function names) for the script
	VariableList m_lVariableList;
	// The list of all errors for the script
	ErrorList m_lErrorList;
	// The list of all tokens for the script
	TokenList m_lTokenList;

public:
	// The constructor of the CParser class, this requires a TokenList (std::list<CToken>) as an argument
	CParser(TokenList lTokenList);
	// Returns true if the variable exists on the variable list, false otherwise
	bool VariableExists(std::string sVariable);
	// Runs the actual parser
	void Run();
};