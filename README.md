CommandEngine
=============

About
-----

A simple library to allow parsing of arbitary strings and then executing them in a direct fashion. 
Great for implementing a command and control structure for serial input/output and for a 
telnet library. (See Telnet Library to see the two of them in action)

Really helpful for creating programs that parse input of somesort and then act on the input. The 
nice thing about the library is it supports macros to allow you to type less. It acts more as a 
command line interpreter than anything fancy. It does not do math or anysuch thing and does not
allow for advanced funcitonality of calling one function from another. That is beyond the scope
of this library and probably not useful in an embedded enviroment.

The engine will parse commands as follows:
[command] [arg1] [arg2] [arg3] [arg4] [...]

[command] "[arg1 with spaces in it]" [arg2] "[arg3 with even more spaces in it]" [arg4] [...]

Items with a $ in front will be checked to see if it is a macro. If it is it will be replaced.
analogWrite $rgbPin $high --> analogWrite A3 1

The macro replacement is the very first step. A macro is defined by calling: set
set ag analogWrite

echo $ag --> analogWrite

The same example as above:
$ag $rgbPin $high --> analogWrite A3 1

As you can see a powerful way to shorten typing if you wish to use it. Macros can also be defined in code.

Two built in functions exist:
set --> allows the definition of new macros
help --> prints out help messages about the commands and a list of all commands

Example Usage
-------------

Incredibly easy to use.



	CommandEngine::CommandLineEngine eng;

	void Command_AnalogWrite( Print * printer, CommandEngine::Tokens & tk, CommandEngine::CommandLineEngine::FunctionFlags ff )
	{
		// Do something in here
	}

	DECLARE_COMMAND(Command_digitalWrite)
		// Do your digitalwrite stuff in here
	DECLARE_COMMAND_END


	void startup() {


	eng.registerMacro("high","1");
	eng.registerMacro("low","0");

	eng.registerFunction("analogWrite", Command_AnalogWrite);
	eng.registerFunction("digitalWrite", Command_digitalWrite);

	}

	void loop() {

		String input;
		
		eng.execute(input, Serial);
	}


The above snippet of code will register two macros that represent high and low in the commands. The registerFunction will 
register a custom function to be called by the user. 

The user will interact by sending a string as follows: analogWrite A0 $high
Step 1) The string is read for macros and becomes : analogWrite A0 1
Step 2) The function: Command_AnalogWrite is called with Tokens: A0, 1

A simple and effective way of parsing a command and executing it with minimal input from you!


Recommended Components
----------------------
I highly recommend the Telnet library. They work wonderfully together.

Circuit Diagram
---------------
Only software

Learning Activities
-------------------
The tokenizer can work standalone. It supports things such as quotation marks. 
The Tokens class is a singlely linked list. It supports forward iteration with iterators. A little more light weight than
using <list> from the standard c++ library. It also has a built in iterator for doing some nifty things. See the example code.


The CommandLineEngine class only ever does a linear search for commands and macros. If you want you could do a quick sort or use a 
hash rather than just a linear vector. I chose a linear vector since how many commands are you really planning on having in the system.
A linear search will be faster for most arrays that have less than 20 or so definitions.


Telnet Server
=============


About
-----

A telnet server allows you to connect to the spark core and use a command line like entry of text. It is a basic server and
provides minimal functionality on its own. It does remove the telnet commands and codes from the string allowing you to 
never have to worry about the telnet protocol itself. 

It is very easy to setup and use. I included the CommandEngine library in the telnet library as they go hand in hand. The 
CommanEngine Library can be used without the telnet library. The telnet library can be used without the CommandEngine library.
However using the Telnet library without the commandengine lib is more difficult and you have to now write your own parser and
function do-er. 

This code has been tested with putty: http://www.chiark.greenend.org.uk/~sgtatham/putty/download.html
It has also been tested with the linux telnet command. (man telnet)


Example
-------

You need to define a function that will be used as a callback whenever the server detects that it has input. The definition 
of the function is:
	typedef void (*processInputCallBack)(Server *,String , int );

In other words:

	void processInput( Telnet::Server * ts, String txt, int flags );

Flags is defined as one of the following values:
	enum Callbackflags {
		CBF_WelcomeMsg 	= 	0x1,
		CBF_InputCmd	=	0x2,
		CBF_Error	= 	0x4,
		CBF_CmdPrompt	= 	0x8,
	}

I currently do not OR them together so a simple switch statement will surfice.

To actually use the server the code will look something like this:

	Telnet::Server tServer(8000);	

	void setup() {

		// Set the function to be called when the telnet server process the input
		tServer.setProcessCallback(processData);

		// Start the server and connect
		tServer.connect();
		
		// End Telnet server setup
	}

	void loop() {

		tServer.poll();
	}

Recommended Components
----------------------
Works with the CommandEngine library very well. It is included already in this code for you!

Circuit Diagram
---------------
Only software

Learning Activities
-------------------
The Telnet server could be adjusted to work with certain more limited terminals if you wish. As of now it does not support line
editing from the client. It expects the client to send complete lines of text to it. The server itself does some work behind
the scenes to shield you from some of the vagaries of the telnet protocol. Unless you want to learn about telnet protocol 
nothing really needs to be done.

However if you want to add features like tab completion then you will need to modify its telnet protocol class. I don't recommend
it unless you want to learn some weird features and have to deal with different clients. 

Why
---
If you don't know what telnet is than I highly recommend that you learn a little about it! Do not worry about the protocol. The
reason this library was written was to allow me to interact with my programs in a better way than reading from the serial port.
I have never connected my SparkCore to the serial port and my cores are placed in not easy to reach places. It is far easier to 
debug my code from a command line like interface rather than using a serial interface. A http one could also be made and I 
considered that however for it to be interactive at a reasonible speed it would require Ajax and other fancy stuff that seems
more overkill than necessary. 


