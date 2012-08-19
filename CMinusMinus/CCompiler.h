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

class CCompiler
{
	TokenList m_lTokenList;
public:
	CCompiler(TokenList lTokenList);
	void Run();
};