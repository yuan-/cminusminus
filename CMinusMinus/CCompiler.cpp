//==============================================================================
//
// File: CCompiler.cpp
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
// 
// The CCompiler class outputs the output code off the token list, after the token
// list being parsed and checked by the CParser.
// 
//==============================================================================

#include "CCompiler.h"
#include "CLogger.h"
#include <fstream>

// Holds a list of all functions and their parameters that need to be called
std::vector<std::pair<int, ParameterList>> CCompiler::m_lAssemblyFunctionList;

// Pushes back a function on the m_lAssemblyFunctionList
void CCompiler::AddFunction(int iFunction, ParameterList lParameterList)
{
	m_lAssemblyFunctionList.push_back(make_pair(iFunction, lParameterList));
}

// Runs the compiler
void CCompiler::Run()
{
	#if _DEBUG
	CLogger::Write("\n* Starting the compilation process:");
	#endif

	// The outputstream for the output file
	std::ofstream assemblyOutput;
	// Open the assembly file where we will write to
	assemblyOutput.open ("input.asm");

	// Include the required file for the MessageBox() function
	assemblyOutput << "include 'FlatAssembler/INCLUDE/win32ax.inc'\n";

	// Actual assembly starts here
	assemblyOutput << ".code\n";
	assemblyOutput << "start:\n";

	// Loop through all the functions we're supposed to call and write them to the file
	for(size_t i = 0; i < m_lAssemblyFunctionList.size(); i++)
	{
		if(m_lAssemblyFunctionList[i].first == MESSAGEBOX_FUNCTION)
		{
			assemblyOutput << "\tinvoke	MessageBox,HWND_DESKTOP,\"" << m_lAssemblyFunctionList[i].second[0].m_sValue.c_str() << "\",\"" << m_lAssemblyFunctionList[i].second[1].m_sValue.c_str() << "\",MB_OK\n";
		}
	}

	// Don't forget to exit the process
	assemblyOutput << "\tinvoke  ExitProcess,0\n";
	assemblyOutput << ".end start\n";

	// Close the file
	assemblyOutput.close();

	// Now run the Assembler using the input file as a parameter
	#if _DEBUG
	system("FlatAssembler\\FASM.exe input.asm");
	#else 
	// If we're doing this in the release mode of the compiler, redirect the output to > null (similar to dev/null on Linux)
	system("FlatAssembler\\FASM.exe input.asm > nul:");
	#endif
}