//==============================================================================
//
// File: Main.cpp
// Project: CMinusMinus
// Author(s): Matthias Van Eeghem (matthias@van-eeghem.com)
// License: See LICENSE in root directory
//
// The main file for the project, entry point for the CMinusMinus.
//
//==============================================================================

#include "CTokenizer.h"
#include "CToken.h"
#include "CLogger.h"
#include "CParser.h"

#include <iostream>
#include <list>

int main(int argc, char * argv[])
{
	#if _DEBUG
	CLogger::Write("* Opening source file: %s", argv[1]);
	#endif

	// Do we have enough arguments passed to the CMinusMinus compiler?
	if(argc < 2)
	{
		CLogger::Write("* Not enough arguments. No source file to open.");
		exit(1);
	}

	// Initialise the tokenizer
	CTokenizer oTokenizer = CTokenizer(argv[1]);
	oTokenizer.Run();

	CParser oParser = CParser(oTokenizer.GetTokenList());
	oParser.Run();

	// Stop the console from closing
	std::getchar();
	return 0;
}