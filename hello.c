#include <stdio.h>
#include <stdio.h>
#include <Serial.h>
#include <math.h>
#include <Devices.h>
#include "string.h"

char* application_id = "TEST_APPLICATION_1";
int call_counter = 0;
IOParam outgoingSerialPortReference; // TODO I think this is an accurate name
IOParam incomingSerialPortReference;

/*
// http://mirror.informatimago.com/next/developer.apple.com/documentation/mac/Devices/Devices-320.html
Read more: http://stason.org/TULARC/os-macintosh/programming/7-1-How-do-I-get-at-the-serial-ports-Communications-and-N.html#ixzz4cIxU3Tob this one is only useful for enumerating ports
// https://developer.apple.com/library/archive/documentation/mac/pdf/Devices/Serial_Driver.pdf
Serial implementation:

https://opensource.apple.com/source/gdb/gdb-186.1/src/gdb/ser-mac.c?txt another example of a serial library
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

// TODO: handle all OSErr - they are all unhandled at the moment

void setupPBControlForSerialPort(short serialPortShort) {

    CntrlParam cb;
    cb.ioCRefNum = serialPortShort; // TODO: this is always 0
    cb.csCode = 8; // TODO: not sure what this is for
    cb.csParam[0] = stop10 | noParity | data8 | baud9600; // TODO: can we achieve higher than 9600 baud?
    OSErr err = PBControl ((ParmBlkPtr) & cb, 0); // PBControl definition: http://mirror.informatimago.com/next/developer.apple.com/documentation/mac/Networking/Networking-296.html
    // second param there is async -- could be OK for what we are doing? TODO: investigate
    char errMessage[100];
    sprintf(errMessage, "err:%d\n", err);
    printf(errMessage);
    if (err < 0) {

        return;
    }

}

// all of the "inbound" stuff here is TODO
void setupSerialPort(const char *name) {
#define MODEM_PORT_OUT   "\p.AOut"
#define MODEM_PORT_IN    "\p.AIn"
#define PRINTER_PORT_OUT "\p.BOut"
#define PRINTER_PORT_IN  "\p.BIn"

    const char* serialPortOutputName = "";
    const char* serialPortInputName = "";

    if (strcmp (name, "modem") == 0) {

        serialPortOutputName = MODEM_PORT_OUT;
        serialPortInputName = MODEM_PORT_IN;
    } else if (strcmp (name, "printer") == 0) {

        serialPortOutputName = PRINTER_PORT_OUT;
        serialPortInputName = MODEM_PORT_IN;
    } else {

        return;
    }

    short serialPortOutput = 0; // TODO: why is this always 0? is this right?
    short serialPortInput = 1; // TODO: not realy sure what this should be - just incrementing from the last item here

    OSErr err = MacOpenDriver(serialPortOutputName, &serialPortOutput); // results in 0 - i think this is good
    char errMessage[100];
    sprintf(errMessage, "err:%d\n", err);
    printf(errMessage);

    if (err < 0) {

        return;
    }

    err = MacOpenDriver(serialPortInputName, &serialPortInput); // results in -43 fnfErr: File not found

    sprintf(errMessage, "err:%d\n", err);
    printf(errMessage);

    if (err < 0) {

        return;
    }

    setupPBControlForSerialPort(serialPortOutput);
    setupPBControlForSerialPort(serialPortInput); // TODO: not really sure if this is right for inputs

    outgoingSerialPortReference.ioRefNum = serialPortOutput;
    incomingSerialPortReference.ioRefNum = serialPortInput; // TODO: not really sure if this is right for inputs
}

/*
need code for reading a serial port, maybe this is right
untested

is PBRead what we want? (From Inside Macintosh Volume II-185)
PBRead attempts to read ioReqCount bytes from the device driver having the reference number ioRefNum, and transfer them to the data buffer pointed to by ioBuffer.
 The drive number, if any, of the device to be read from is specified by ioVRefNum. Afterthereadiscompleted,the position is returned in ioPosOffset and the number
 of bytes actually read is returned in ioActCount.
*/
char* readSerialPort() {
    
    printf("attempting to read from serial port...\n");

    char *stringToReadFromSerial;

    incomingSerialPortReference.ioBuffer = (Ptr) stringToReadFromSerial;
    incomingSerialPortReference.ioReqCount = 1; // TODO: we need an expected receive length. coprocessor.js needs to supply set chunk sizes?
    // TODO pretend for now - i'm assuming we need to chunk reads and figure out when to stop in the future
    // TODO: we need to figure out the proper format for coprocessor, but it needs to be able to tell us that there is more data and that we should call
    // PBRead repetively.

    OSErr err = PBRead((ParmBlkPtr)& incomingSerialPortReference, 0);
    char errMessage[100];
    sprintf(errMessage, "err:%d\n", err);
    printf(errMessage);
    // TODO: also need a mechanism for ensuring we are on the right application_id/call_id on the response (thinking of if there are multiple coprocessor.js applications
    // running on the old Mac at once?)
    // TODO: we will need to implement a locking mechanism on coprocessor where it must be aware that it can only write one response at a time

    return stringToReadFromSerial;
}


OSErr writeSerialPort(const char* stringToWrite) {

    outgoingSerialPortReference.ioBuffer = (Ptr) stringToWrite;
    outgoingSerialPortReference.ioReqCount = strlen(stringToWrite);
    printf("attempting to write string to serial port\n");

    // PBWrite Definition From Inside Macintosh Volume II-185:
    // PBWrite takes ioReqCount bytes from the buffer pointed to by ioBuffer and attempts to write them to the device driver having the reference number ioRefNum.
    // The drive number, if any, of the device to be written to is specified by ioVRefNum. After the write is completed, the position is returned in ioPosOffset and the number of bytes actually written is returned in ioActCount.
    OSErr err = PBWrite((ParmBlkPtr)& outgoingSerialPortReference, 0); // second param is async, TODO: investigate // returns -51:rfNumErr: refnum error
    char errMessage[100];
    sprintf(errMessage, "err:%d\n", err);
    printf(errMessage);
    return err;
}

// TODO: what else does this function need?
// maybe setting application ID in a local state rather than tracking it globally
void setupCoprocessor(const char *name) {

    setupSerialPort(name);

    return;
}

OSErr closeSerialPort() {

    OSErr err = MacCloseDriver(outgoingSerialPortReference.ioRefNum);
    char errMessage[100];
    sprintf(errMessage, "err:%d\n", err);
    printf(errMessage);
    return err;
}

void writeToCoprocessor(char* operation, char* operand) {

    const char* messageTemplate = "%s;;;%s;;;%s;;;%s;;@@&&"; // see: https://github.com/CamHenlin/coprocessor.js/blob/main/index.js#L25
    char call_id[32];
    char messageToSend[10000]; // TODO need to not over/under allocate -- or figure out how to tell coprocessor that we are going to send chunks if we have to

    sprintf(call_id, "%d", call_counter++);

    // application_id is globally defined for now, how will that work in a library?
    sprintf(messageToSend, messageTemplate, application_id, call_id, operation, operand);

    OSErr err = writeSerialPort(messageToSend);
    char errMessage[100];
    sprintf(errMessage, "writeToCoprocessor err:%d\n", err);
    printf(errMessage);
}

char* sendProgramToCoprocessor(char* program) {

    writeToCoprocessor("PROGRAM", program);
    char* response = readSerialPort();

    // TODO: rather than just returning the response we should check to see if it was successful then return a boolean
    return response;
}

char* callFunctionOnCoprocessor(char* functionName, char* parameters) {

    const char* functionTemplate = "%s&&&%s";
    char functionCallMessage[10000]; // TODO
    sprintf(functionCallMessage, functionTemplate, functionName, parameters); // TODO: should parameters be a list that we join?

    writeToCoprocessor("FUNCTION", functionCallMessage);
    char* response = readSerialPort();

    return response;
}

char* callEvalOnCoprocessor(char* toEval) {

    writeToCoprocessor("EVAL", toEval);
    char* response = readSerialPort();

    return response;
}

int main(int argc, char** argv) {

    printf("CoprocessorJS Test App: starting...\n");
    printf("CoprocessorJS Test App: setting up printer port...\n");

    setupCoprocessor("modem"); // could also be "printer", modem is 0 in PCE settings

    // TODO: figure out how to move these to Mac resource files. I'm pretty sure the format is pretty easy to follow and extract from
    // started taking a stab at some of this in the Makefile
    // look at https://github.com/clehner/Browsy/blob/5c3241acd781c0555d02ed329297cb83d66bf9c2/src/uri/about.c#L76 and the corresponding makefile
    const char* TEST_INDEX = "const _ = require('lodash')\nclass SimpleNodeThing {\n  static isEmpty (variable) {\n    return _.isEmpty(variable)\n  }\n  isEmpty (variable) {\n    return this.constructor.isEmpty(variable)\n  }\n}\nmodule.exports = SimpleNodeThing";
    const char* TEST_PACKAGE = "{\n  \"name\": \"test\",\n  \"version\": \"1.0.0\",\n  \"description\": \"\",\n  \"main\": \"index.js\",\n  \"scripts\": {\n    \"test\": \"\"\n  },\n  \"author\": \"\",\n  \"license\": \"ISC\",\n  \"dependencies\": {\n    \"lodash\": \"^4.17.21\"\n  }\n}";

    // TODO: set length as required
    char programCall[10000];

    sprintf(programCall, "index.js@@@%s&&&package.json@@@%s", TEST_INDEX, TEST_PACKAGE);

    printf("CoprocessorJS Test App: attempting to send program to Coprocessor...\n");
    sendProgramToCoprocessor(programCall);
    
    printf("CoprocessorJS Test App: attempting to call lodash isEmpty function on Coprocessor...\n");
    char* jsFunctionResponse = callFunctionOnCoprocessor("isEmpty", "test");

    printf("CoprocessorJS Test App: attempting to eval code block on Coprocessor...\n");
    callEvalOnCoprocessor("console.log(this)");

    printf("CoprocessorJS Test App: isEmpty function call response:\n");
    printf(jsFunctionResponse);

    printf("\ntest complete - process any key to quit\n");
    getchar();
    closeSerialPort();

    return 0;
}
