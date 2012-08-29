//==============================================================================
//
// File: NativeFunctions.cpp
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
// 
// The source file containing all native functions for the language.
// 
//==============================================================================

#include "NativeFunctions.h"

// The power function executes base^exponent (both parameters are floats)
CReturnValue power(ParameterList lParameterList)
{
	double fBase = lParameterList[0].m_fValue;
	double fExponent = lParameterList[1].m_fValue;

	return CReturnValue(VARIABLE_TYPE_FLOAT, (float) pow(fBase, fExponent));
}

// The squareroot function, returns the squareroot of the float parameter
CReturnValue squareroot(ParameterList lParameterList)
{
	double fValue = lParameterList[0].m_fValue;

	return CReturnValue(VARIABLE_TYPE_FLOAT, (float) sqrt(fValue));
}