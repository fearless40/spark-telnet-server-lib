

#include "defs.h"
#include <spark_wiring_string.h>
#include "TelnetCommands.h"
#include "TelnetServer.h"
#include "BasicTelnetServerHandler.h"
#include <string.h>


using namespace Telnet;

String BasicTelnetHandler::OnFirstConnection(){

	//return "\xFF\xFD\x22"; // Force linemode
	// putty and telnet (linux) both drop you into edit local mode. No need to negotiate.
	// 1) IAC DO LINEMODE --> IAC WILL LINEMODE
	// 2) IAC DONT FLOWCONTROL	--> IAC WONT FLOWCONTROL
	// 3) IAC DONT ECHO	--> IAC WONT FLOWCONTROL

	return " ";
}

// Client wants to start negotiation do something
String BasicTelnetHandler::Will(char c) {

	switch( c ) {
		case LINEMODE:
			return "will linemode \xFF\xFA\x22\x01\x01\xFF\xF0 \xFF\xFA\x22\xFE\x02\xFF\xF0"; //Do Followed by bellow
			//return "LINEMODE \xFF\xFA\x22\x01\x01\xFF\xF0\xFF\xFA\x22\xFE\x02";	//IAC SB LINEMODE MODE EDIT IAC SE / IAC SB LINEMODE DONT FORWARDMASK IAC SE
		case TERMINAL_TYPE:
			return "\xFF\xFE\x18"; // IAC DONT TERMINAL //"\xFF\xFD\x18 \xFF\xFA\x18\x01\xFF\xF0"; // IAC DO Terminal Type With subnegotiaion mode
		case ECHO:
			return "\xFF\xFE\x01";  //dont want echo
		case SUPPRESS_GO_AHEAD:
			return "\xFF\xFD\x03"; //surpress it
		case FLOWCONTROL:
			return "\xFF\xFE\x21"; //dont want flow control
		//case TERMINAL_SPEED:
		//	return "\xFF\xFD\x20";	// Allow negotiation of terminal speed

		default:
			// Reject all other options
			char ret[] = {IAC,DONT,c,0};
			return ret;
	};
}

// Client wants me to do do something
String BasicTelnetHandler::Do(char c) {
	switch (c) {
		case FLOWCONTROL:
			return "\xFF\xFC\x21";	// IAC WONT FLOWCONTROl (the server never flow controls));
		case ECHO:
			return "\xFF\xFC\x01";	// IAC WONT ECHO
		case LINEMODE:
			return "\xFF\xFD\x22";	// IAC DO LINEMODE	(ok to negotiate linemode commands)
		case SUPPRESS_GO_AHEAD:
			return "\xFF\xFB\x03"; // IAC DO suppress go ahead
		default:
			// Reject all other options

			char ret[] = {IAC,WONT,c,0};
			return ret;
	};

}

// Client does not want to do the following
String BasicTelnetHandler::Wont(char c) {
	// Only valid option
	char ret[] = {IAC,DONT,c,0};
	return ret;

}

// Client does not want me to do the following
String BasicTelnetHandler::Dont(char c) {
	// only valid option
	char ret[] = {IAC,WONT,c,0};
	return ret;

}

String BasicTelnetHandler::SB(const Telnet::Server::CmdBuffer & buf) {

	// Buf[0] == Option
	// Buf[1] == Option sub negotians

	// begin negotiating linemode options
	switch (buf[0] )
	{
		case LINEMODE:
			// Currently force the client to what we want and do no true negotiation.
			// If the client wants other settings set then we ignore it
			if ( buf[1] == MODE && buf[2] == EDIT | MODE_ACK) { // Client is responding that they turned on linemode for us
				// Do nothing
				return "LINEMODE EDIT SET\r\n";
			}
			else {
				return " ";//"\xFF\xFA\x22\x01\x01\xFF\xF0\xFF\xFA\x22\xFE\x02"; //IAC SB LINEMODE MODE EDIT IAC SE / IAC SB LINEMODE DONT FORWARDMASK IAC SE
			}
		break;
		case TERMINAL_TYPE:
		/*	{
			String deb("Term Type: ");
			if( buf[1] == 0 ) {
				char val[32];
				memset(val,0,32);
				for( int i = 2; i < 32; ++i )
				{
					if( buf[i] == IAC || buf[i] == 0 )
						break;

					val[i-2] = buf[i];
				}

				return deb + val + "\xFF\xFA\x18\x01\xFF\xF0";
			}
			}
			return "Negotiate Term Type";*/
		default:
			return " ";

	}

};
