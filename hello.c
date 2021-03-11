#include <stdio.h>
#include <Serial.h>
#include <math.h>
#include <Devices.h>
#include "string.h"

char* application_id = "TEST_APPLICATION_1";
int call_counter = 0;
IOParam pb2; // TODO name this better, what does pb stand for?

/*
Read more: http://stason.org/TULARC/os-macintosh/programming/7-1-How-do-I-get-at-the-serial-ports-Communications-and-N.html#ixzz4cIxU3Tob

Serial implementation: 

https://opensource.apple.com/source/gdb/gdb-186.1/src/gdb/ser-mac.c?txt 
http://mirror.informatimago.com/next/developer.apple.com/documentation/mac/Devices/Devices-320.html
*/ 

// notes from above article:
// You can use OpenDriver, SetReset, SetHShake, SetSetBuf, SerGetBuf and
// the other Serial Manager functions on these drivers.

// To write to the
// serial port, use FSWrite for synchronous writes that wait until all is
// written, or PBWrite asynchronously for queuing up data that is supposed
// to go out but you don't want to wait for it.  

// At least once each time
// through your event loop, you should call SerGetBuf on the in driver
// reference number you got from OpenDriver, and call FSRead for that many
// bytes - neither more nor less.


void setupSerialPort(short refNum)
{ 
#define MODEM_PORT_OUT   "\p.AOut" 
#define MODEM_PORT_IN    "\p.AIn" 
#define PRINTER_PORT_OUT "\p.BOut" 
#define PRINTER_PORT_IN  "\p.BIn"
    
    const char* nameStr = "";
    switch (refNum)
    {
        case aoutRefNum:
            nameStr = MODEM_PORT_OUT;
            break;
        case boutRefNum:
            nameStr = PRINTER_PORT_OUT;
            break;   

        // input device not valid for writing data
		// TODO, figure this out -- assuming we'll need these to be able to read back responses?
        /*
        case ainRefNum:
            nameStr = MODEM_PORT_IN;
            break;
        case binRefNum:
            nameStr = MODEM_PORT_IN;
            break;         
        */
            
        default:
            return;        
    }
    
    short serialPort = 0;
    OSErr err = MacOpenDriver(nameStr, &serialPort);
   
    if (err < 0) {
		
		return;
	}
    
	// TODO: cb seems sort of unused here - what is this for? figure this out
	// TODO at least create a better name for it
    CntrlParam cb;
    cb.ioCRefNum = serialPort;
    cb.csCode = 8;
    cb.csParam[0] = stop10 | noParity | data8 | baud9600; // TODO: can we achieve higher than 9600 baud?
    err = PBControl ((ParmBlkPtr) & cb, 0);

    if (err < 0) {
		
		return;
	}
            
    pb2.ioRefNum = serialPort;
}


OSErr writeSerialPort(const char* str) { 

    char str2[10000]; // TODO: why are we creating a new string? to put a linebreak at the end or something? investigate
    sprintf(str2, "%s\n", str);

    pb2.ioBuffer = (Ptr) str2;
    pb2.ioReqCount = strlen(str2);
    
    OSErr err = PBWrite((ParmBlkPtr)& pb2, 0);          

    if (err < 0) {

		return err;
	}
// TODO: we should only close on program exit, we need to be waiting on reads    
//    err = MacCloseDriver(serialPort);
//
//    return err;
}

// TODO: what else does this function need?
void setupCoprocessor(short refNum) {
	
	setupSerialPort(refNum);
	
	return;
}

// TODO: call on exit
OSErr closeSerialPort() {
	
	OSErr err = MacCloseDriver(pb2.ioRefNum);

    return err;
}

void writeToCoprocessor(char* operation, char* operand) {
	
	const char* messageTemplate = "%s;;;%s;;;%s;;;%s"; // see: https://github.com/CamHenlin/coprocessor.js/blob/main/index.js#L25
	char call_id[32];
	char messageToSend[10000]; // TODO need to not over/under allocate
	
	sprintf(call_id, "%d", call_counter++);

	// application_id is globally defined for now, how will that work in a library?
	sprintf(messageToSend, messageTemplate, application_id, call_id, operation, operand);
	
	OSErr err = writeSerialPort(messageToSend);
}

void sendProgramToCoprocessor(char* program) {
	
	writeToCoprocessor("PROGRAM", program);
	
	// TODO: handle success after program is loaded on coprocessor
	
	return;
}

void callFunctionOnCoprocessor(char* functionName, char* parameters) {
	
	const char* functionTemplate = "%s&&&%s"; 
	char functionCallMessage[10000]; // TODO
	sprintf(functionCallMessage, functionTemplate, functionName, parameters); // TODO: should parameters be a list that we join?
	
	writeToCoprocessor("FUNCTION", functionCallMessage);
	
	// TODO: get function response
	
	return;
}

void callEvalOnCoprocessor(char* toEval) {

	writeToCoprocessor("EVAL", toEval);
	
	// TODO: get eval response
	
	return;
}

int main(int argc, char** argv) {

	setupCoprocessor(boutRefNum);

	// TODO: figure out how to move these to Mac resource files. I'm pretty sure the format is pretty easy to follow and extract from
	const char* TEST_INDEX = "const _ = require('lodash')\nclass SimpleNodeThing {\n  static isEmpty (variable) {\n    return _.isEmpty(variable)\n  }\n  isEmpty (variable) {\n    return this.constructor.isEmpty(variable)\n  }\n}\nmodule.exports = SimpleNodeThing";
	const char* TEST_PACKAGE = "{\n  \"name\": \"test\",\n  \"version\": \"1.0.0\",\n  \"description\": \"\",\n  \"main\": \"index.js\",\n  \"scripts\": {\n    \"test\": \"\"\n  },\n  \"author\": \"\",\n  \"license\": \"ISC\",\n  \"dependencies\": {\n    \"lodash\": \"^4.17.21\"\n  }\n}";

	// TODO: set length as required
	char programCall[10000];

	sprintf(programCall, "index.js@@@%s&&&package.json@@@%s", TEST_INDEX, TEST_PACKAGE);
	
	sendProgramToCoprocessor(programCall);
	callFunctionOnCoprocessor("isEmpty", "test");
	callEvalOnCoprocessor("console.log(this)");

	printf("\n(Press Enter to quit)\n");  
	getchar();

	return 0;
}