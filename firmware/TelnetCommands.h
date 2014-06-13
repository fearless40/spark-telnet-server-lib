
#ifndef __TELNET_TELNETCOMMANDS__
#define __TELNET_TELNETCOMMANDS__

namespace Telnet {

enum Codes {
	cNULL = 0,		// No operation
	LF = 10, 		// Line feed, Moves the printer to the next print line, keeping the same horizontal position.
	CR = 13,		// Carriage Return, 	Moves the printer to the left margin of the current line.
 	BEL= 7, 		// BELL, Produces an audible or visible signal (which does NOT move the print head.
	BS = 8, 		// Back Space, 	Moves the print head one character position towards the left margin. [On a printing devices this mechanism was commonly used to form composite characters by printing two basic characters on top of each other.]
	HT = 9, 		// Horizontal Tab, Moves the printer to the next horizontal tab stop. It remains unspecified how either party determines or establishes where such tab stops are located.
	VT = 11, 		// Vertical Tab , Moves the printer to the next vertical tab stop. It remains unspecified how either party determines or establishes where such tab stops are located.
	FF = 12, 		// Form Feed, Moves the printer to the top of the next page, keeping the same horizontal position. [On visual displays this commonly clears the screen and moves the cursor to the top left corner.
	DEL = 127		// Delete key
};


enum Commands {
	SE 	= 	240, //	End of subnegotiation parameters.
	NOP	= 	241, //	No operation
	DM 	=	242, //	Data mark. Indicates the position of a Synch event within the data stream. This should always be accompanied by a TCP urgent notification.
	BRK	= 	243, //	Break. Indicates that the "break" or "attention" key was hit.
	IP 	=	244, //	Suspend, interrupt or abort the process to which the NVT is connected.
	AO 	=	245, //	Abort output. Allows the current process to run to completion but do not send its output to the user.
	AYT	= 	246, //	Are you there. Send back to the NVT some visible evidence that the AYT was received.
	EC 	=	247, //	Erase character. The receiver should delete the last preceding undeleted character from the data stream.
	EL 	=	248, //	Erase line. Delete characters from the data stream back to but not including the previous CRLF.
	GA 	=	249, //	Go ahead. Used, under certain circumstances, to tell the other end that it can transmit.
	SB 	=	250, //	Subnegotiation of the indicated option follows.
	WILL= 	251, //	Indicates the desire to begin performing, or confirmation that you are now performing, the indicated option.
	WONT= 	252, //	Indicates the refusal to perform, or continue performing, the indicated option.
	DO 	=	253, //	Indicates the request that the other party perform, or confirmation that you are expecting the other party to perform, the indicated option.
	DONT= 	254, //	Indicates the demand that the other party stop performing, or confirmation that you are no longer expecting the other party to perform, the indicated option.
	IAC	= 	255  //	Interpret as command
};

enum Options {
	BINARY 				= 0,			// Allow binary transmission of data
	LINEMODE 			= 34,			// Start of line mode commands
	TERMINAL_TYPE		= 24,			// What kind of terminal the client is emulating or is
	ECHO				= 1,			// Should the server send back commands to the client
	SUPPRESS_GO_AHEAD	= 3,			// Suppress sending of GO AHEAD terminal commands (we are using a full duplex system)
	FLOWCONTROL			= 33,			// Flow control (not necessary)
	TERMINAL_SPEED		= 32			// How fast should it send
	// There are many others that are ignored
};

enum LineModeSubOptions {
	MODE 		= 1,
	FORWARDMASK = 2,
	SLC  		= 3,
	EOF 		= 236,
	SUSP 		= 237,
	ABORT 		= 238
};

enum LineModeModeSubOptions {
   EDIT      =	1,
   TRAPSIG   =	2,
   MODE_ACK  =	4,
   SOFT_TAB  =	8,
   LIT_ECHO  =	16
};

enum ANSIEscape {
	AE_ESC		= 27,		// Escape used for special commands
	AE_ESC2		='[',		// Part 2 of escape sequence
	AE_End		='m',
	AE_Sep		=';'
};



}

#endif
