//==============================================================================
//
// File: CParameter.h
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
// 
// The CParameter structure represents a function parameter for the CFunction.
// 
//==============================================================================

#pragma once

#include <vector>

// Enum that represents the all possible parameter types
enum eParameterTypes
{
	PARAMETER_TYPE_INTEGER,
	PARAMETER_TYPE_FLOAT,
	PARAMETER_TYPE_STRING
};

struct CParameter
{
	// Since the CParameter struct can hold three types of variables (floats, integers and strings)
	// we need three seperate variables for each of these types.
	std::string m_sValue;
	int m_iValue;
	double m_fValue;

	// The type this CVariable object holds
	eParameterTypes m_eType;

	// The CParameter class has three constructors, one for each parameter type
	// The constructor for a parameter that represents an integer
	CParameter::CParameter(eParameterTypes eType, int iValue): m_eType(eType), m_iValue(iValue) { }
	// The constructor for a parameter that represents a float
	CParameter::CParameter(eParameterTypes eType, float fValue): m_eType(eType), m_fValue(fValue) { }
	// The constructor for a parameter that represents a string
	CParameter::CParameter(eParameterTypes eType, std::string sValue): m_eType(eType), m_sValue(sValue) { }
};

typedef std::vector<CParameter> ParameterList;