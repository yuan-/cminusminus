//==============================================================================
//
// File: CFunction.h
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
// 
// Each function (native or not) for the language is represented by a CFunction
// structure. It holds the function name, the parameter types the function expects
// and a pointer to the function it should call (only for native functions).
// 
//==============================================================================

#include "CParameter.h"
#include "CReturnValue.h"

#pragma once

struct CFunction
{
	// The function name
	std::string m_sName;
	// A list of parameter types the function expects
	std::vector<eParameterTypes> m_lParameterTypes;
	// Do we have to check for types for this function?
	bool m_bTypeSensitive;

	// A pointer to the function that is supposed to be called
	// Only for native functions
	CReturnValue (*m_pFunctionToCall) (ParameterList);
};

typedef std::vector<CFunction> FunctionList;