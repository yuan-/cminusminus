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

#pragma once

#include <vector>
#include "CParameter.h"

// This enum holds all possible assembly functions that can be called (these are actually Win32 functions)
enum eAssemblyFunctions
{
	MESSAGEBOX_FUNCTION // MessageBox(), in win32ax.inc
};

// Typedef to make the function list more readable
typedef std::vector<std::pair<int, ParameterList>> AssemblyFunctionList;

class CCompiler
{
	// Holds a list of all functions and their parameters that need to be called
	static AssemblyFunctionList m_lAssemblyFunctionList;

public:
	// Pushes back a function on the m_lAssemblyFunctionList
	static void AddFunction(int iFunction, ParameterList lParameterList);
	// Runs the compiler
	static void Run();
};