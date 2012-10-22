//==============================================================================
//
// File: CFunctionWrapper.h
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
// 
// The CFunctionWrapper is as the name implies a wrapper around the CFunction class.
// It holds multiple CFunction objects, each native function is represented by one.
// It also has a couple of utility functions in regards to the functions.
// 
//==============================================================================

#pragma once

#include "CFunction.h"
#include "CFunctionCallAttempt.h"

class CFunctionWrapper
{
	// The list of all functions for the script
	static FunctionList m_lFunctionList;

public:
	// This method calls an existing function with a parameter list
	static CFunctionCallAttempt CallFunction(std::string sFunctionName, ParameterList lParameterList);
	// This method registers all natives for the language
	static void RegisterNatives();
	// This method returns true if a function exists, false otherwise
	static bool FunctionExists(std::string sFunctionName);
	// This method registers a function with the script
	static void RegisterFunction(std::string sName, CReturnValue (*pFunctionToCall) (ParameterList), std::vector<eParameterTypes> eParameterTypes, bool bTypeSensitive = false);
};