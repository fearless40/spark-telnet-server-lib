#include "telnet-server-lib/telnet-server-lib.h"


Telnet::Server tServer(8000);
CommandEngine::CommandLineEngine cleg;

// Called when the telnet server is ready to send you data
void processData( Telnet::Server * ts, String txt, int flags ) {

	switch (flags) {
		case Telnet::Server::CBF_WelcomeMsg:		//on first connect this gets called
			ts->println("Hello and welcome to test server");
			ts->forceCommandPrompt();
			break;

		case Telnet::Server::CBF_Error:				//on overflow or error
			ts->print(txt);
			ts->forceCommandPrompt();
			break;

		case Telnet::Server::CBF_CmdPrompt:			// when the command promt is ready
			ts->print(">>");
			break;

	 default:
		// Process the command
		cleg.execute( txt, ts );					// call the CommandLineEngine
		ts->forceCommandPrompt();					// force the command prompt
	}
}

// Helper functions

struct MapperPinID {
	const char * name;
	int			 value;
};

MapperPinID PinIDMapping[] = {
	{"DO",D0},
	{"D1",D1},
	{"D2",D2},
	{"D3",D3},
	{"D4",D4},
	{"D5",D5},
	{"D6",D6},
	{"D7",D7},
	{"A0",A0},
	{"A1",A1},
	{"A2",A2},
	{"A3",A3},
	{"A4",A4},
	{"A5",A5},
	{"A6",A6},
	{"A7",A7},
	{"LED1",LED1}
};

struct MapperPinMode {
	const char * name;
	PinMode		 value;
};

MapperPinMode PinModesMapping[] = {
	{"output",			OUTPUT},
	{"input",			INPUT},
	{"input_pullup",	INPUT_PULLUP},
	{"input_pulldown",	INPUT_PULLDOWN}
};

#define STATIC_ARRAY_SIZE(x) (sizeof(x)/sizeof(*x))

int ConvertStringToPinID( String pin ) {
	for( unsigned int i=0; i < STATIC_ARRAY_SIZE(PinIDMapping); ++i)
	{
		if( pin == PinIDMapping[i].name )
			return PinIDMapping[i].value;
	}
	return -1;
}

PinMode ConvertStringToPinMode( String pinMode )
{
	for( unsigned int i=0; i < STATIC_ARRAY_SIZE(PinModesMapping); ++i)
	{
		if( pinMode == PinModesMapping[i].name )
			return PinModesMapping[i].value;
	}
	return AN_INPUT;
}

String ConvertPinModeToString( int pinMode ) {
	for( unsigned int i=0; i < STATIC_ARRAY_SIZE(PinModesMapping); ++i)
	{
		if( pinMode == PinModesMapping[i].value )
			return PinModesMapping[i].name;
	}
	return "unknown";
}

#undef STATIC_ARRAY_SIZE

// End helper functions


// Custom commands for the telnet server

void CommandFunction_PinMode( Print * printer, CommandEngine::Tokens & tk, CommandEngine::CommandLineEngine::FunctionFlags ff)
{
	if( ff == CommandEngine::CommandLineEngine::FF_HelpText )
	{
		printer->println("Help with PinMode command:");
		printer->println("PinMode [pin name] [pin mode]");
		printer->println("[pin name] = D0-7, A0-7, LED1");
		printer->println("[pin mode] = output, input , input_pullup, input_pulldown");
		printer->println("Will change the pin mode if it can.");
		printer->println("PinMode [pin name]");
		printer->println("Will read what the current pin is and return the current value.");
		return;
	}

	String pinName; String pMode;
	if( !tk.next( pinName ) ) {
		printer->println("Insufficent arguments. For more information type: help PinMode");
		return;
	}

	int pinID = ConvertStringToPinID(pinName);

	if( pinID == -1 ){
		printer->println("Unknown Pin id specified. Please specify a correct pin.");
		return;
	}

	if( !tk.next(pMode) )  {
		// Attempt to read the pMode from
		printer->println("Pin mode reading is not implemented.");
		return;
	}
	else{
		// Set the pin mode
		PinMode pM = ConvertStringToPinMode(pMode);
		if( pM == AN_INPUT ) {
			printer->println("Unknown pin mode specified. Please specify a correct pin mode.");
			return;
		}

		pinMode( pinID, pM );
		printer->print("Set the pin: "); printer->print(pinName); printer->println(" ");
	}
}

void CommandFunction_DigitalWrite(Print * printer, CommandEngine::Tokens & tk, CommandEngine::CommandLineEngine::FunctionFlags ff)
{
	if( ff == CommandEngine::CommandLineEngine::FF_HelpText )
	{
		printer->println("Help with DigitalWrite command:");
		printer->println("DigitalWrite [pin name] $high/$low/0/1");
		printer->println("[pin name] = D0-7, A0-7");
		printer->println("Will write to the pin the specified value.");
		return;
	}

	String pinName; String pValue;
	if( !tk.next( pinName ) ) {
		printer->println("Insufficent arguments. For more information type: help DigitalWrite");
		return;
	}

	int pinID = ConvertStringToPinID(pinName);

	if( pinID == -1 ){
		printer->println("Unknown Pin id specified. Please specify a correct pin.");
		return;
	}

	if( !tk.next(pValue) )  {
		// Attempt to read the pMode from
		printer->println("Need a value to set the pin.");
		return;
	}

	int value  = (int)pValue.toInt();

	digitalWrite( pinID, value );
	printer->print("Set pin: "); printer->print(pinName); printer->print(" to value: "); printer->println( value );
}

void CommandFunction_AnalogWrite(Print * printer, CommandEngine::Tokens & tk, CommandEngine::CommandLineEngine::FunctionFlags ff)
{
	if( ff == CommandEngine::CommandLineEngine::FF_HelpText )
	{
		printer->println("Help with AnalogWrite command:");
		printer->println("AnalogWrite [pin name] 0-255");
		printer->println("[pin name] = A0-7");
		printer->println("Will write to the pin the specified value.");
		return;
	}

	String pinName; String pValue;
	if( !tk.next( pinName ) ) {
		printer->println("Insufficent arguments. For more information type: help AnalogWrite");
		return;
	}

	int pinID = ConvertStringToPinID(pinName);

	if( pinID == -1 ){
		printer->println("Unknown Pin id specified. Please specify a correct pin.");
		return;
	}
	else if( pinID < FIRST_ANALOG_PIN) {
		printer->println("Unable to set specified pin to analogwrite value. You may have specified a digital pin.");
		return;
	}

	if( !tk.next(pValue) )  {
		// Attempt to read the pMode from
		printer->println("Need a value to set the pin.");
		return;
	}

	int value  = (int)pValue.toInt();

	analogWrite( pinID, value );
	printer->print("Set pin: "); printer->print(pinName); printer->print(" = "); printer->println( value );
}

void CommandFunction_AnalogRead(Print * printer, CommandEngine::Tokens & tk, CommandEngine::CommandLineEngine::FunctionFlags ff)
{
	if( ff == CommandEngine::CommandLineEngine::FF_HelpText )
	{
		printer->println("Help with AnalogRead command:");
		printer->println("AnalogRead [pin name] ");
		printer->println("[pin name] = A0-7");
		printer->println("Will read the value of the pin.");
		return;
	}

	String pinName; String pValue;
	if( !tk.next( pinName ) ) {
		printer->println("Insufficent arguments. For more information type: help AnalogRead");
		return;
	}

	int pinID = ConvertStringToPinID(pinName);

	if( pinID == -1 ){
		printer->println("Unknown Pin id specified. Please specify a correct pin.");
		return;
	}
	else if( pinID < FIRST_ANALOG_PIN) {
		printer->println("Unable to read specified pin to analogread value. You may have specified a digital pin.");
		return;
	}


	int value  = analogRead(pinID);

	printer->print("Value of pin: "); printer->print(pinName); printer->print(" = "); printer->println( value );
}

void CommandFunction_DigitalRead(Print * printer, CommandEngine::Tokens & tk, CommandEngine::CommandLineEngine::FunctionFlags ff)
{
	if( ff == CommandEngine::CommandLineEngine::FF_HelpText )
	{
		printer->println("Help with DigitalRead command:");
		printer->println("DigitalRead [pin name] ");
		printer->println("[pin name] = D0-D7, A1-7");
		printer->println("Will read the value of the pin.");
		return;
	}

	String pinName; String pValue;
	if( !tk.next( pinName ) ) {
		printer->println("Insufficent arguments. For more information type: help DigitalRead");
		return;
	}

	int pinID = ConvertStringToPinID(pinName);

	if( pinID == -1 ){
		printer->println("Unknown Pin id specified. Please specify a correct pin.");
		return;
	}


	int value  = digitalRead(pinID);

	printer->print("Value of pin: "); printer->print(pinName); printer->print(" = "); printer->println( value == 0 ? "low" : "high" );
}


void CommandFunction_echo(Print * printer, CommandEngine::Tokens & tk, CommandEngine::CommandLineEngine::FunctionFlags ff)
{
	if( ff == CommandEngine::CommandLineEngine::FF_HelpText )
	{
		printer->println("Help with Echo command:");
		printer->println("echo [value1] [value2] [value3] [value...]");
		printer->println("[value] =  either a macro $ or a literal value");
		printer->println("Mostly for debugging the engine.");
		return;
	}

	String arg; int c = 0;
	while( tk.next(arg) ){
		printer->print("Argument "); printer->print(c++); printer->print(": "); printer->println(arg);
	}
}

void CommandFunction_information(Print * printer, CommandEngine::Tokens & tk, CommandEngine::CommandLineEngine::FunctionFlags ff) {

	if( ff == CommandEngine::CommandLineEngine::FF_HelpText )
	{
		printer->println("Lists some useful information.");
		printer->println("No arguments needed for this function.");
		return;
	}

	// Get information like the IP address
	printer->print( "IP Address: "); printer->println( Network.localIP() );
	printer->print( "SSID: "); printer->println( Network.SSID() );
	printer->print( "Uptime: "); printer->print( millis() / 1000 ); printer->println( " seconds");
}

void setup()
{
	// Command Engine setup

    // Register macros
    cleg.registerMacro("high", "1");
    cleg.registerMacro("low","0");

	// Register custom functions with the engine
    cleg.registerFunction("echo", 			CommandFunction_echo );
    cleg.registerFunction("PinMode", 		CommandFunction_PinMode);
    cleg.registerFunction("digitalWrite", 	CommandFunction_DigitalWrite);
    cleg.registerFunction("digitalRead",	CommandFunction_DigitalRead);
    cleg.registerFunction("analogRead", 	CommandFunction_AnalogRead);
    cleg.registerFunction("analogWrite", 	CommandFunction_AnalogWrite);
    cleg.registerFunction("information", 	CommandFunction_information);

    // End Command engine setup

	// Telnet Server setup

	// Set the function to be called when the telnet server process the input
	tServer.setProcessCallback(processData);

	// Start the server and connect
    tServer.connect();

    // End Telnet server setup

}

void loop()
{
	// Need it to call it in your loop function
	tServer.poll();
}
