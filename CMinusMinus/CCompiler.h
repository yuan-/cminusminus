//==============================================================================
//
// File: CCompiler.h
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
// 
// The CCompiler class outputs the output code off the token list, after the token
// list being parsed and checked by the CParser.
// 
//==============================================================================

#include <list>
#include "CToken.h"
#include "CError.h"

class CCompiler
{
	ErrorList m_lErrorList;
	TokenList m_lTokenList;

public:
	// The constructor of the CCompiler class, this requires a TokenList (std::list<CToken>) as an argument
	CCompiler(TokenList lTokenList);
	void Run();
};