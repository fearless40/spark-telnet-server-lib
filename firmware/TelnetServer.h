#ifndef __TELNET_TELNETSERVER__
#define __TELNET_TELNETSERVER__

#include "StaticCharStream.h"
#include <spark_wiring_tcpclient.h>
#include <spark_wiring_tcpserver.h>
#include "SimpleTimer.h"


// If you create your own telnet handler than change this define.
// The better way would be to use a template however that may force the entire class
// to be in the header file.
#define TelnetHandler BasicTelnetHandler

namespace Telnet {

class Server : public TCPServer
{
public:
	// Buffer size
	static const int CmdBufferSize = 16;	// Not necessary to change. 

	enum Colors {
		Black 	= 0,
		Red		= 1,
		Green	= 2,
		Yellow	= 3,
		Blue	= 4,
		Magenta	= 5,
		Cyan	= 6,
		White	= 7,
		Bright	= 1,
		Foreground = 30,
		Background = 40
	};

	enum Options {
		OP_Read_BufferSize,			// What is the read buffer size
		OP_Write_BufferSize,		// Not used
		OP_Client_TimeOut,			// How fast does the server conisder a null client disconnected
	};

	enum Callbackflags {
		CBF_WelcomeMsg 	= 	0x1,	// What text you want as the welcome message as a user connects
		CBF_InputCmd	=	0x2,	// The user typed in text and hit the enter key
		CBF_Error		= 	0x4,	// Only called when the buffer overflows. 
									//	Todo: Add a buffer overflow flag and allow the user to determine the best course of action
		CBF_CmdPrompt	= 	0x8,	// What text to be displayed for the command prompt
	};

	// Define Callbacks
	// Input Server: [in] gives a pointer to the server
	// Input String: [in] String of text that the user typed
	// Input int:	 [in] a set of Callback flags specifying what is going on
	typedef void (*processInputCallBack)(Server *,String , int );

	// CmdBuffer is a static stream like device. Allows one to process streams of data
	//		without worring about fixed array sizes. More efficent than using a String
	//		data type as no data allocation or deallocation is done.
	//		Very simple type mostly for ease of use.
	typedef Telnet::StaticCharStream<CmdBufferSize> CmdBuffer;

	Server(int port);
	Server();
	~Server();

	// Call in your main loop to check for new inputs
	//	If a enter key is found it will then call the process event function
	//	Return:
	//		true = still connected to the client
	//		false = no connection to client
	bool poll();

	// Connect (start the server and wait for a connection)
	//	Returns:
	//		True: succesful start up
	//		False: failed
	bool connect();

	// Disconnet from the client
	void disconnect();

	// Set an option
	//	meaning of value changes for different opt
	//	opt = OP_Read_BufferSize, value = size in bytes
	//	opt = OP_Write_BufferSize, value = size in bytes
	//	opt = OP_Client_TimeOut, value = milli seconds before assuming they are gone
	void setOption(Options opt, unsigned int value );


	// Return an option
	//	see above
	unsigned int getOption(Options opt);

	// Colors
	//	Allows the server to send ANSI escape color codes to the client
	void	setColor( Colors col, bool bright = false, bool foreground = true  );
	void	setDefaultColor();


	// Force resend prompt
	//	Forces the command prompt to be displayed no matter what
	void	forceCommandPrompt();


	// Set the callback function
	//	This is where the main processing of the telnet server will be done
	//	by the client. If you want to respond to input such as:
	//		Client--> Hello
	//			processInputCallBack (this, "Hello\0", 5)
	//				Server--> Yo!
	//		Client<-- Yo!
	//	The callback only gets called when the client sends user data entry.
	//	Therefore things like echos and commands are handled by the server and
	//		dont'necessary need to be handled by the callback
	void setProcessCallback( processInputCallBack pCall );


protected:

	// Called everytime new data has accumulated
	void processStream();

	// Override to handle telnet commands
	// 	Strip the commands from the stream and respond to the commands as needed.
	void handleCommands();

	void allocateBuffers();

	void refreshTimeoutTimer();

	// Helper functions for just cleanliness
	inline char consume() {
		return mClient.read();
	}

	// Helper functions
	inline bool canConsume() {
		return mClient.available() > 0 ? true : false;
	}

private:
	enum State {
		Uninitalized = 0,
		InitalizedNoConnection,
		FirstConnectionToClient,
		Disconnected,
		ActiveConnection,
		ReceivingCommand,
		AwaitingCommandResponse,
	};

	State state;

	TCPClient mClient;

	// Time out and port information
	int	mPort;
	unsigned long mTimeoutDelay;
	Telnet::SimpleTimer mClientTimer;

	// Telnet Command Processing
	char	mCurrentCommand;
	CmdBuffer	mCmdBuffer;
	const int CmdTimerMaxTime = 5000; // max of 500 milli seconds for a command to come through.
									 // If it takes longer then abort processing of commands
	Telnet::SimpleTimer mCmdTimer;

	// User Buffer Processing
	char * mReadBuffer;
	unsigned int mReadBufSize;
	unsigned int mReadEndPos;

	// Callbacks
	processInputCallBack mProcessedCallback;

};
};

#endif
