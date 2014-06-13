
#ifndef __TELNET_BASICTELNETHANDLER__
#define __TELNET_BASICTELNETHANDLER__

namespace Telnet {

class BasicTelnetHandler {
public:
	// Called to negotiate parameters on first connection (gives the server a first chance to ask for stuff)
	static String OnFirstConnection();
	static String Will(char c);
	static String Do(char c);
	static String SB(const Server::CmdBuffer & buf);
	static String Wont(char c);
	static String Dont(char c);
};

};

#endif
