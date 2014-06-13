
#ifndef __COMMANDLINE_COMMANDLINEENGINE__
#define __COMMANDLINE_COMMANDLINEENGINE__

#include <vector>

namespace CommandLine {

class CommandLineEngine
{

public:

	enum FunctionFlags {
		FF_HelpText,	// The function was called by the help function
		FF_Normal		// The function was called normally
	};

	// User function definition:
	// [IN] Print *: never is null. If no printer is specified a nullprinter will be sent which will
	//				 consume the bytes without any output.
	// [IN] Tokens &: The command tokenized and macros expanded for you
	// [IN] FunctionFlags: One of flags defined in the enum FunctionFlags
	typedef void (*UserFunction)(Print *, Tokens &, FunctionFlags);

	// Register your function with the class
	void registerFunction( String funcName, UserFunction func );

	// Allow you to register a macro from code
	void registerMacro( String macroName, String value );

	// Call this when a string needs to be executed.
	// [IN] String cmdLine: the string of text that you want processed
	// [IN] Print * printer: the class that will act as your output. If not specified
	//		a NullPrinter will be used instead. (see the cpp file for more info)
	bool execute( String cmdLine, Print * printer = nullptr );


protected:

	// Internal Commands
	// They could be defined in the mUserFunctions but makes it much harder
	// 	as they need to access internal data structures
	void help(Print * printer, Tokens & tk, FunctionFlags ff);
	void set(Print * printer, Tokens & tk, FunctionFlags ff);

	// Helpers
	void doMacros(Tokens & tk);

	struct UserFunctionDef {
		String name;
		UserFunction func;
	};

	struct UserMacroDef {
		String name;	// Stored without the dollar sign
		String value;
	};

	typedef std::vector< UserFunctionDef > 	UserFunctions;
	typedef std::vector< UserMacroDef >		UserMacros;

	UserFunctions 	mUserFunctions;
	UserMacros		mUserMacros;

	// Helper Functinos for the vectors
	UserMacros::iterator findMacro(String txt);
	UserFunctions::iterator findFunction(String txt);
};



};

#endif
