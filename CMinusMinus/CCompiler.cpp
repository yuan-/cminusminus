//==============================================================================
//
// File: CCompiler.cpp
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
// 
// The CCompiler class outputs the output code off the token list, after the token
// list being parsed and checked by the CParser.
// 
//==============================================================================

#include "CCompiler.h"

CCompiler::CCompiler(TokenList lTokenList)
{
	m_lTokenList = lTokenList;
}

void CCompiler::Run()
{
	for(TokenList::iterator iterator = m_lTokenList.begin(); iterator != m_lTokenList.end(); iterator++)
	{
	}
}