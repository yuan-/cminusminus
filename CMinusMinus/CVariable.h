//==============================================================================
//
// File: CVariable.h
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
//
// The CVariable class holds a variable that can be found in the source. The struct
// can hold three types of variables, floats, integers and strings. It also holds
// the name of the variable and the type of variable.
//
//==============================================================================

#pragma once

#include <string>

// This enum holds all possible types the CVariable struct can hold
enum eVariableTypes
{
	VALUE_TYPE_INTEGER,
	VALUE_TYPE_FLOAT,
	VALUE_TYPE_STRING
};

struct CVariable
{
	// Since the CVariable struct can hold three types of variables (floats, integers and strings)
	// we need three seperate variables for each of these types.
	std::string m_sValue;
	int m_iValue;
	double m_fValue;

	// The name of the variable
	std::string m_sValueName;
	// The type this CVariable object holds
	eVariableTypes m_eType;
	// This bool is set to true if the variable has been declared and defined, false otherwise
	// 'int iUnassigned;' means that it's declared but not yet defined, 'int iAssigend = 42;' means
	// it's declared and defined.
	bool m_bHasBeenAssignedAnything;

	CVariable::CVariable(): m_bHasBeenAssignedAnything(false) { }
};

typedef std::list<CVariable> VariableList; 