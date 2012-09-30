//==============================================================================
//
// File: CFunctionWrapper.cpp
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
// 
// The CFunctionWrapper is as the name implies a wrapper around the CFunction class.
// It holds multiple CFunction objects, each native function is represented by one.
// It also has a couple of utility functions in regards to the functions.
// 
//==============================================================================

#include "CFunctionWrapper.h"
#include "NativeFunctions.h"
#include "Util.h"

#include <sstream>

// The list of all functions for the script
FunctionList CFunctionWrapper::m_lFunctionList;

// This method registers a function with the script
void CFunctionWrapper::RegisterFunction(std::string sName, CReturnValue (*pFunctionToCall) (ParameterList), std::vector<eParameterTypes> lParameterTypes)
{
	// Set up a CFunction object and push it back onto the function list
	CFunction oFunction;
	oFunction.m_sName = sName;
	oFunction.m_lParameterTypes = lParameterTypes;
	oFunction.m_pFunctionToCall = pFunctionToCall;

	m_lFunctionList.push_back(oFunction);
}

// This method registers all natives for the language
void CFunctionWrapper::RegisterNatives()
{
	// squareroot(float fValue);
	std::vector<eParameterTypes> lRequiredParameterTypes;
	lRequiredParameterTypes.push_back(PARAMETER_TYPE_FLOAT);
	RegisterFunction("squareroot", squareroot, lRequiredParameterTypes);

	// power(float fBase, float fExp);
	lRequiredParameterTypes.clear();
	lRequiredParameterTypes.push_back(PARAMETER_TYPE_FLOAT);
	lRequiredParameterTypes.push_back(PARAMETER_TYPE_FLOAT);
	RegisterFunction("power", power, lRequiredParameterTypes);

	lRequiredParameterTypes.clear();
	lRequiredParameterTypes.push_back(PARAMETER_TYPE_STRING);
	lRequiredParameterTypes.push_back(PARAMETER_TYPE_STRING);
	RegisterFunction("messageBox", messageBox, lRequiredParameterTypes);
}

// This method returns true if a function exists, false otherwise
bool CFunctionWrapper::FunctionExists(std::string sFunctionName)
{
	// Loop through all the functions
	for(size_t i = 0; i < m_lFunctionList.size(); i++)
	{
		// Do we have a matching name?
		if(m_lFunctionList[i].m_sName == sFunctionName)
			return true;
	}

	// Nothing found, return false
	return false;
}

// This method calls an existing function with a parameter list
CFunctionCallAttempt CFunctionWrapper::CallFunction(std::string sFunctionName, ParameterList lParameterList)
{
	// The object we'll return to the script
	CReturnValue oReturnValue;

	// Loop through all the functions
	for(FunctionList::iterator iterator = m_lFunctionList.begin(); iterator != m_lFunctionList.end(); iterator++)
	{
		// Do we have a function name match?
		if((*iterator).m_sName == sFunctionName)
		{
			// Do we have enough parameters? (are the parameter lists equally big?)
			if((*iterator).m_lParameterTypes.size() != lParameterList.size())
			{
				// Setup the error message
				std::stringstream ssErrorMessage;
				ssErrorMessage << sFunctionName << " expects " << (*iterator).m_lParameterTypes.size() << " parameter(s), got " << lParameterList.size() << ".";

				return CFunctionCallAttempt(ssErrorMessage.str());
			}
			
			// Current parameter number
			int iCurrentParameterNumber = 1;

			// Check if every parameter has the correct type
			for(unsigned int i = 0; i < lParameterList.size(); i++, iCurrentParameterNumber++)
			{
				if(lParameterList[i].m_eType != (*iterator).m_lParameterTypes[i])
				{
					// The types of this parameter differ
					std::stringstream ssErrorMessage;
					ssErrorMessage << "Parameter " << iCurrentParameterNumber << " has a bad type (expected " << GetTypeAsString((*iterator).m_lParameterTypes[i]) << ", got " << GetTypeAsString(lParameterList[i].m_eType) << ", in function call " << sFunctionName << ")";

					return CFunctionCallAttempt(ssErrorMessage.str());
				}
			}

			// Call the actual function
			oReturnValue = (*iterator).m_pFunctionToCall(lParameterList);
			break;
		}
	}

	// Return a valid CFunctionCallAttempt object
	return CFunctionCallAttempt(oReturnValue);
}