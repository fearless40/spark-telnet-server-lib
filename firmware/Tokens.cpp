
#include "defs.h"
#include <spark_wiring_string.h>
#include <string.h>
#include "Tokens.h"

using namespace CommandLine;

String Tokens::iterator::mValueReturn;

Tokens & Tokens::Tokenize(Tokens & tk, String txt, bool doQuotes, const char * seperators )
{
	bool inQuotes = false;
	int	 start = 0;
	int  nbrSeperators = strlen(seperators);
	tk.erase();

	// remove the trailing and leading spaces
	txt.trim();

	// Special case of single character
	if( txt.length() == 1 ) {
		tk.add(txt);
		tk.makeReadyForUser();
		return tk;
	}

	for( int index = 0; index < txt.length(); ++index )
	{
		if( doQuotes && txt.charAt(index) == '\"'  )
		{
			if( inQuotes == true ) {

				if( index - start > 0)	// Check for the case of ""
					tk.add( txt.substring( start, index ) ); // Do not include the quotes into the text stream
				inQuotes = false;
				start = index + 1;
				continue;
			}
			else {
				inQuotes = true;
				start = index + 1; // Do not include the quotes into the stream
			}
		}

		if( !inQuotes )
		{
			for( int i = 0; i < nbrSeperators; ++i )
			{
				if(txt.charAt(index) == seperators[i] )
				{ //Found a seperator character

					if( index - start > 0 )	// Check for ' '
						tk.add( txt.substring(start, index) ); // Do not include the seperator character

					start = index + 1;
					break; //leave the seperator loop
				}
			}
		}
	}

	// Need to add the very last argument to the list
	if( start < txt.length() ) {
		tk.add( txt.substring(start) );
	}

	tk.makeReadyForUser();
	return tk;
}
