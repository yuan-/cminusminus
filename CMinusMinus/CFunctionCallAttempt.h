//==============================================================================
//
// File: CFunctionCallAttempt.h
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
// 
// Each time CFunctionWrapper::CallFunction() is called it returns a CFunctionCallAttempt
// object. If an error occured when calling the function, m_bErrorOccured will be set
// to true and the error message will be stored in m_sErrorMessage.
// 
// If no error occured, a CReturnValue object is stored in m_oReturnValue.
//
//==============================================================================

#pragma once

struct CFunctionCallAttempt
{
	// If an error occured while calling the function, the m_bErrorOccured flag is set to true
	bool m_bErrorOccured;
	// If an error occured, the error message is stored in this variable
	std::string m_sErrorMessage;
	// If no error occured, the CReturnValue object is stored in this variable
	CReturnValue m_oReturnValue;

	// The CFunctionCallAttempt struct has two constructors, one for a valid function call, and one for an invalid one
	// The constructor for a valid function call. It take a CReturnValue parameter
	CFunctionCallAttempt::CFunctionCallAttempt(CReturnValue oReturnValue): m_bErrorOccured(false), m_oReturnValue(oReturnValue) { }
	// The constructor for an invalid function call. It takes an error message as parameter
	CFunctionCallAttempt::CFunctionCallAttempt(std::string sErrorMessage): m_bErrorOccured(true), m_sErrorMessage(sErrorMessage) { }
};