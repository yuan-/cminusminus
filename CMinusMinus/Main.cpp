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

#include <iostream>
#include <list>

int main(int argc, char * argv[])
{
	// Do we have enough arguments passed to the CMinusMinus?
	if(argc < 2)
	{
		CLogger::Write("* Not enough arguments. No source file to open.");
		exit(1);
	}

	CLogger::Write("Opening source file: %s", argv[1]);

	// Initialise the tokenizer
	CTokenizer oTokenizer = CTokenizer(argv[1]);
	oTokenizer.Run();
	std::list<CToken> lTokenList = oTokenizer.GetTokenList();

	// Stop the console from closing
	std::getchar();
	return 0;
}