
#include "defs.h"
#include "spark_wiring_print.h"
#include "spark_wiring_string.h"
#include "Tokens.h"
#include "CommandLineEngine.h"


using namespace CommandLine;

class NullPrinter : public Print {
public:
	size_t write(uint8_t) { return 0; }
    size_t write(const char *str) { return 0; }
    size_t write(const uint8_t *buffer, size_t size) { return 0; }
};

NullPrinter nullprinter;

void CommandLineEngine::registerFunction( String funcName, CommandLineEngine::UserFunction func )
{
	UserFunctions::iterator it = findFunction(funcName);
	if( it == mUserFunctions.end() )
	{
		UserFunctionDef uf = { funcName, func };
		mUserFunctions.push_back(uf);
	}
	else{
		it->func = func;
	}
}

void CommandLineEngine::registerMacro( String macroName, String value )
{
	UserMacros::iterator it = findMacro(macroName);
	if( it == mUserMacros.end() ) {
		UserMacroDef um = {macroName, value};
		mUserMacros.push_back(um);
	}
	else {
		it->value = value;
	}
}

#define quickprint(x) if(printer) { printer->println(x); }

bool CommandLineEngine::execute( String cmdLine, Print * printer )
{

	if( cmdLine.length() <= 0 )
		return false;

	Tokens tk;


	if( Tokens::Tokenize(tk,cmdLine).isEmpty() )
		return false;

	// Replace the macros now
	// doMacros uses external iterator so the internal iterator of tk is untouched

	doMacros( tk );

	// Now find the function and call it
	// But first check if it is one of the built in functions


	String func;
	if (!tk.next(func))
		return false;

	// Remove trailing weird space issues
	func.trim();


	if( func.equalsIgnoreCase("set") ) {
		// Built in function
		set( printer, tk, FF_Normal );
	}
	else if ( func.equalsIgnoreCase("help") ) {
		// Built in function
		help( printer, tk, FF_Normal );
	}
	else {
		bool foundfunction = false;
		// Now Search for the function and call the user function if
		//  we can find it

		for( UserFunctions::iterator it = mUserFunctions.begin(); it != mUserFunctions.end(); ++it )
		{

			if( func.equalsIgnoreCase( it->name) ) {
				foundfunction = true;
				it->func( 	printer ? printer : &nullprinter,
							tk,
							FF_Normal);
				break;
			}
		}
		if( !foundfunction ) {
			if(printer) {
				printer->print("Unable to find the specified command: ");
				printer->println( func );
			}
			return false;
		}

	}

	return true;
}

void CommandLineEngine::doMacros(Tokens & tk)
{
	for( Tokens::iterator it = tk.begin(); it != tk.end(); ++it)
	{
		String & v = it;
		if( v[0] == '$' ) {
			String macro = v.substring(1);
			UserMacros::iterator it = findMacro(macro);

			if( it == mUserMacros.end() )
				continue;
			else
				v = it->value;
		}
	}
}


void CommandLineEngine::help(Print * printer, Tokens & tk, FunctionFlags ff)
{
	if (!printer)
		return;

	UserFunctions::iterator it;

	String arg;
	if( tk.next(arg) )
	{
		if( arg.equalsIgnoreCase("help") )
		{
			printer->println("Type help without any arguments to learn more.");
			return;
		}
		else if( arg.equalsIgnoreCase("set") )
		{
			set(printer, tk, FF_HelpText);
			return;
		}

		it = findFunction(arg);
		if( it == mUserFunctions.end() )
		{
			printer->print("Unable to locate the function: ");
			printer->println(arg);
			return;
		}
		// Call the function and tell it to display its help text
		it->func(printer, tk, FF_HelpText);
		return;
	}
	else
	{
		printer->println("Function Name: help");
		printer->println("Type help <function name> to learn more about that function.");
		printer->println("The following functions are defined:");
		printer->println("help");
		printer->println("set");

		// List all the functions
		for( it = mUserFunctions.begin(); it != mUserFunctions.end(); ++it)
		{
			printer->println(it->name);
		}
	}
}

void CommandLineEngine::set(Print * printer, Tokens & tk, FunctionFlags ff)
{
	if( ff == FF_HelpText ) {
		if( printer ) {
			printer->println("Function Name: Set");
			printer->println("Description: Use it to set a macro. When using set do not include the $ sign.");
			printer->println("To use a macro type the $macro name. For instance the following commands.");
			printer->println("set redpin D7");
			printer->println("pin $redpin high");
			printer->println("The set function syntax is as follows:");
			printer->println("set [macroname] [value]");
		}
		return;
	}

	String arg, val;
	if( !tk.next(arg) || !tk.next(val) )
		quickprint("Invalid syntax. Type help set")


	UserMacros::iterator it = findMacro(arg);
	if( it == mUserMacros.end() ) {
		UserMacroDef um = { arg, val};
		mUserMacros.push_back( um );
	}
	else{
		it->value = val;
	}
}

	// Helper Functinos for the vectors
CommandLineEngine::UserMacros::iterator CommandLineEngine::findMacro(String txt)
{
	// Searches for the string exactly as it is seen
	for( UserMacros::iterator it = mUserMacros.begin(); it != mUserMacros.end(); ++it)
	{
		if( txt.equalsIgnoreCase( it->name ))
			return it;
	}

	return mUserMacros.end();
}

CommandLineEngine::UserFunctions::iterator CommandLineEngine::findFunction(String txt)
{
	for( UserFunctions::iterator it = mUserFunctions.begin(); it != mUserFunctions.end(); ++it)
	{
		if( txt.equalsIgnoreCase( it->name ))
			return it;
	}

	return mUserFunctions.end();
}

#undef quickprint
