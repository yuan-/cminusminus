//==============================================================================
//
// File: CReturnValue.h
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
// 
// The CReturnValue structure represents the return value for the CFunction struct.
// 
//==============================================================================

#pragma once

#include "CVariable.h"

struct CReturnValue
{
	// Since the return value can hold three types of variables (floats, integers and strings)
	// we need three seperate variables for each of these types.
	std::string m_sValue;
	int m_iValue;
	double m_fValue;

	// The type this CReturnValue object holds
	eVariableTypes m_eType;

	// The CReturnValue struct has 4 constructors
	// Default constructor, empty CReturnValue object
	CReturnValue::CReturnValue() { }
	// The constructor for a return value that represents an integer
	CReturnValue::CReturnValue(eVariableTypes eType, int iValue): m_eType(eType), m_iValue(iValue) { }
	// The constructor for a return value that represents a float
	CReturnValue::CReturnValue(eVariableTypes eType, float fValue): m_eType(eType), m_fValue(fValue) { }
	// The constructor for a return value that represents a string
	CReturnValue::CReturnValue(eVariableTypes eType, std::string sValue): m_eType(eType), m_sValue(sValue) { }
};