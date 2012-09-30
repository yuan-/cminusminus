//==============================================================================
//
// File: NativeFunctions.h
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
// 
// The header file containing all native functions for the language.
// 
//==============================================================================

#include "CReturnValue.h"
#include "CParameter.h"

// The power function executes base^exponent (both parameters are floats)
CReturnValue power(ParameterList);
// The squareroot function, returns the squareroot of the float parameter
CReturnValue squareroot(ParameterList);
// The messageBox function, outputs a mesagebox
CReturnValue messageBox(ParameterList);