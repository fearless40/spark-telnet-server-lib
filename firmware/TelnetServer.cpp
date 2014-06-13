
#include "defs.h"
#include "TelnetCommands.h"
#include "TelnetServer.h"
#include "BasicTelnetServerHandler.h"


using namespace Telnet;

Server::Server(int port) : mPort(port),
	mProcessedCallback(nullptr),
	mReadBuffer(nullptr),
	mReadBufSize(128),
	mReadEndPos(0),
	mTimeoutDelay(36000000), //10 minutes
	state(Uninitalized),
	TCPServer(port)
	{}


Server::Server() : mPort(23),
	mProcessedCallback(nullptr),
	mReadBuffer(nullptr),
	mReadBufSize(128),
	mReadEndPos(0),
	mTimeoutDelay(36000000), //10 minutes
	state(Uninitalized),
	TCPServer(mPort)
	{}

Server::~Server()
{
	delete[] mReadBuffer;
}

bool Server::connect()
{
	if( state == Uninitalized ) {
		allocateBuffers();
	}

	this->begin();

	state = InitalizedNoConnection;

	return true;
}


void Server::disconnect()
{
	mClient.stop();
}

void Server::setOption(Server::Options opt, unsigned int value)
{
	switch( opt ) {
		case OP_Read_BufferSize:
			mReadBufSize = value;
			allocateBuffers();
			break;
		case OP_Client_TimeOut:
			mTimeoutDelay = value;
			break;
	}
}

unsigned int Server::getOption(Server::Options opt)
{
	switch( opt ) {
		case OP_Read_BufferSize:
			return mReadBufSize;
		case OP_Client_TimeOut:
			return mTimeoutDelay;
	};
}


bool Server::poll()
{
	if( state == Uninitalized )
		return false;


	// Check to see if the connection is still active

	if( !mClient.connected() ) {
		state = InitalizedNoConnection;
		mClient = this->available();
	}

	//	If the state is NoConnection then try to see if we are connected
	//		if we are connected then do welcome message and telnet negotiation
	if( state == InitalizedNoConnection && mClient.connected() ) {
		state = FirstConnectionToClient;

		// Start negotiation of telnet parameters
		print(TelnetHandler::OnFirstConnection());

		// Do welcome message
		if(mProcessedCallback)
			mProcessedCallback(this, String(), CBF_WelcomeMsg);

		state = ActiveConnection;

		// mClient.flush();

		// Exit to prevent further processing
		return true;

	}

	int nbrBytes = mClient.available();

	if( nbrBytes > 0 ) {
		int  val =   0xFF & mClient.peek();	// Check the byte to see what it is
		switch (val) {
			case cNULL:
				refreshTimeoutTimer();
				mClient.read();
				return true;
			case IAC:
				state = ReceivingCommand;
				consume();	// remove the IAC command from the que
				break;
		}

		if( state == ReceivingCommand ) {
			handleCommands();
		}

		if( state == ActiveConnection ) {
			refreshTimeoutTimer();
			processStream();
		}
	}
	return true;
}

#define ResetState	mCurrentCommand = 0; state = ActiveConnection; mCmdBuffer.reset();

void Server::handleCommands()
{
	if( mCurrentCommand == 0 ) {
		// No command active
		mCurrentCommand = consume();
		mCmdBuffer.reset();
		mCmdTimer.start();

	}
	else {
		// Check to see the amount of time that has elapsed since we first recieved the command telnet codes
		// if the command is greater than the max allowed time then reset it and deactive command processing
		if( mCmdTimer.elapsed() > CmdTimerMaxTime ) {
			ResetState;
		}
	}

//#define h 	{char c=mClient.peek();char val[32];memset(val,0,32);itoa(c,val,10);print(val);};

	// Use a switch statement to support certain commands

	switch ( mCurrentCommand )
	{
		case WILL:
			if( canConsume() ) {
			// WILL Responses are always 3 bytes long. Send the best response back to the client
			//	print( "WILL: ");
				print( TelnetHandler::Will( consume() ) );
				ResetState;
			}
			break;
		case DO:
			if( canConsume() ) {
			// DO Responses are always 3 bytes long. Send the best response back to the client
			//	print( "DO: ");
				print( TelnetHandler::Do( consume() ) );
				ResetState;
			}
			break;
		case DONT:
			if( canConsume() ) {
			// DONT Responses are always 3 bytes long. Send the best response back to the client
			//	print( "DONT: ");
				print( TelnetHandler::Dont( consume() ) );
				ResetState;
			}
			break;
		case WONT:
			if( canConsume() ) {
			// WONT Responses are always 3 bytes long. Send the best response back to the client
			//	print( "WONT: ");
				print( TelnetHandler::Dont( consume() ) );
				ResetState;
			}
			break;
		case SB:
			// The max this loop can run is defined by Server::CmdTimerMaxTime (may hang your code for a short time)
			//print( "SB: "); h
			//mClient.flush();
			mCmdTimer.start();
			while( canConsume() && !mCmdBuffer.full() && (mCmdTimer.elapsed() < Server::CmdTimerMaxTime) ) {
				mCmdBuffer << consume();
				if( mCmdBuffer.last() == SE ) {
					if( mCmdBuffer.right(1) == IAC ) {
						// A true SE was found
						// Process the SE
						//print( "SB HANDLER: ");
						print( TelnetHandler::SB( mCmdBuffer ) );
						ResetState;
						return;
					}

					// Not a well formed SE ending (missing the IAC or just sent in error)
					//print( "BAD SE:\n ");
					ResetState;
					return;
				}
			}
			//print("SB: Loop did not work");
			// We don't reset the state here as maybe only 5 bytes got sent before the poll function was called.
			//	The goal is to make the system immune to interrupts. Would be so nice to have a C++ yield command....
			break;
		case IP:
			// Send abort signal
			// Not implemented
			// Currently will send a null response back to the client...
		case AYT:
			write( cNULL );
		default:
			// All other commands are not going to send any extra data
			//		therefore we can safely ignore what they are saying
			ResetState;

	}
}

#undef ResetState

void Server::processStream()
{
	bool inEscSequence = false;
	char val = 0;
	while ( canConsume() && mReadEndPos < mReadBufSize )
	{
		if( mClient.peek() == IAC )	// Found a command in here return to regular processing
			return;

		val = consume();

		// Strip out ASCII escape sequences
		if( val == AE_ESC ) {
			inEscSequence = true;
		}
		else if( inEscSequence && (val == AE_End || val == cNULL) )
		{
			inEscSequence = false;
			continue;
		}

		// If regular character
		if( !inEscSequence )
		{
			if( val == CR )	//End of the line
			{
				if( canConsume() ) {	// Eat the LF character or NULL
					val = consume();
				}

				mReadBuffer[mReadEndPos++] = '\0';
				print("\r\n");	// To prevent the cmdline from being erased
				if( mProcessedCallback )
					mProcessedCallback(this, String(mReadBuffer), CBF_InputCmd);

				// Reset the buffer to the beginning
				mReadEndPos = 0;
				return;	// Exit the function
			}

			// Special characters that will be removed
			if( val == BS || val == BEL || val == VT || val == cNULL )
			{
				// Do not save the value into the stream causes errors
				continue;
			}

			if( val == DEL ) {
				// Special logic for putty default
				// On the BackSpace key putty sends the following: DEL
				// For example: user types ech0
				// They meant to type: echo
				// They press the backspace key on ech0 --> ech they then type o
				// so the input looks like echo
				// putty sends the following: ech0DELo  where DEL = ASCII code 127
				// The following code takes that into account
				--mReadEndPos;
				if( mReadEndPos < 0 )
					mReadEndPos == 0;

				continue;
			}

			// Convert horizontal tab into a space
			if( val == HT ) { val = ' '; }

			mReadBuffer[mReadEndPos++] = val;
		}

		if( mReadEndPos + 1 > mReadBufSize ) {
			// Too many characters need to increase the size of the buffer
			// Currently I don't do that, I just report an error
			// Null terminate
			mReadBuffer[mReadEndPos] = '\0';

			// Attempt to process what is in the buffer
			if( mProcessedCallback )
					mProcessedCallback(this, String(mReadBuffer), CBF_Error);

			println("Error: More charactes than the buffer can allow.");
			mReadEndPos = 0; // Reset the buffer
			return;
		}

	}

}

void Server::forceCommandPrompt()
{
	if( mProcessedCallback )
		mProcessedCallback(this, String(), CBF_CmdPrompt);
}

void Server::allocateBuffers()
{
	if( mReadBuffer )
		delete mReadBuffer;

	mReadBuffer = new char[mReadBufSize];
	mReadEndPos = 0;

}

void Server::refreshTimeoutTimer()
{
	mClientTimer.start();
}

void Server::setProcessCallback( processInputCallBack pCall ) {
	if( pCall )
		mProcessedCallback = pCall;
}

void Server::setColor( Colors col, bool bright, bool foreground   )
{
		char c[] = {AE_ESC,AE_ESC2,
		static_cast<char>( (bright) ? 1 : 0 ),
		AE_Sep,
	    static_cast<char>(((foreground) ? Foreground : Background) + (char)col),
	    AE_End, };

		write((unsigned char*)c,sizeof(c));
}

void Server::setDefaultColor()
{
	char val[] = {AE_ESC,AE_ESC2,1,AE_End};
	write( (unsigned char*)val, sizeof(val));
}

#undef ResetState


