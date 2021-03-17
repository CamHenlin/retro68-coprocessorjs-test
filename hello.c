#include <stdio.h>
#include <stdio.h>
#include <Serial.h>
#include <math.h>
#include <Devices.h>
#include "string.h"
#include <stdbool.h>

char* application_id = "TEST_APPLICATION_1";
int call_counter = 0;
IOParam outgoingSerialPortReference;
IOParam incomingSerialPortReference;
const bool PRINT_ERRORS = false;
const bool DEBUGGING = false;

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
    cb.ioCRefNum = serialPortShort; // TODO: this is always 0 - does it matter? should we hard code 0 here? research
    cb.csCode = 8; // TODO: need to look up and document what csCode = 8 means
    cb.csParam[0] = stop10 | noParity | data8 | baud9600; // TODO: can we achieve higher than 9600 baud? - should be able to achieve at least 19.2k on a 68k machine
    OSErr err = PBControl ((ParmBlkPtr) & cb, 0); // PBControl definition: http://mirror.informatimago.com/next/developer.apple.com/documentation/mac/Networking/Networking-296.html

    if (PRINT_ERRORS) {

        char errMessage[100];
        sprintf(errMessage, "err:%d\n", err);
        printf(errMessage);
    }

    if (err < 0) {

        return;
    }
}

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
    short serialPortInput = 0; // TODO: not realy sure what this should be - just incrementing from the last item here

    OSErr err = MacOpenDriver(serialPortOutputName, &serialPortOutput);
    
    if (PRINT_ERRORS) {

        char errMessage[100];
        sprintf(errMessage, "err:%d\n", err);
        printf(errMessage);
    }

    if (err < 0) {

        return;
    }

    err = MacOpenDriver(serialPortInputName, &serialPortInput); // result in 0 but still doesn't work

    if (PRINT_ERRORS) {

        char errMessage[100];
        sprintf(errMessage, "err:%d\n", err);
        printf(errMessage);
    }

    if (err < 0) {

        return;
    }

    // From https://developer.apple.com/library/archive/documentation/mac/pdf/Devices/Serial_Driver.pdf
    // Set baud rate and data format. Note that you only need to set the
    // output driver; the settings are reflected on the input side
    setupPBControlForSerialPort(serialPortOutput);

    outgoingSerialPortReference.ioRefNum = serialPortOutput;
    incomingSerialPortReference.ioRefNum = serialPortInput;
}

char* readSerialPort() {

    if (DEBUGGING) {

        printf("attempting to read from serial port...\n");
    }

    long int byteCount = 0;
    long int lastByteCount = 0;
    short serGetBufStatus;
    char completeStringToReadFromSerial[1024]; // TODO: 1kbyte max window - is this ok? document at least - this would prevent big responses from the nodejs service
    time_t start, end; // setup stall -- do we like this? TODO: research best practices for reading serial ports

    // this loop is to iterate until the byteCount matches for 2 iterations.
    // the idea is that if the byteCount matches, there is no more data being written
    // to the buffer and we can begin our read.
    // this could potentially not work right if writes are slow and the loop executes fast so we mitigate with a timer
    // TODO: look online to see if there are better best practices to implement around this and reading serial buffers
    while (byteCount != lastByteCount || byteCount == 0) {

        time(&start);

        do {

            time(&end);
        } while (difftime(end, start) <= 0.001); // param here is in seconds

        lastByteCount = byteCount;
        serGetBufStatus = SerGetBuf(incomingSerialPortReference.ioRefNum, &byteCount); //  TODO: handle status, should be 0 for success, any other is error. type is short
    }

    incomingSerialPortReference.ioBuffer = (Ptr) completeStringToReadFromSerial;
    incomingSerialPortReference.ioReqCount = byteCount;

    OSErr err = PBRead((ParmBlkPtr)& incomingSerialPortReference, 0); // 0 is sync

    if (PRINT_ERRORS) {

        char errMessage[100];
        sprintf(errMessage, "err:%d\n", err);
        printf(errMessage);
    }

    // NewPtr is malloc for Classic Macintosh http://mirror.informatimago.com/next/developer.apple.com/documentation/mac/Memory/Memory-75.html
    // TODO: this works but is leaking memory - the calling function must deallocate after usage -- consider having calling functions provide a variable to write to
    char *output = NewPtr(strlen(completeStringToReadFromSerial));
    strncpy(output, completeStringToReadFromSerial, strlen(completeStringToReadFromSerial));// - 1);

    return (output);
}


OSErr writeSerialPort(const char* stringToWrite) {

    outgoingSerialPortReference.ioBuffer = (Ptr) stringToWrite;
    outgoingSerialPortReference.ioReqCount = strlen(stringToWrite);
    printf("attempting to write string to serial port\n");

    // PBWrite Definition From Inside Macintosh Volume II-185:
    // PBWrite takes ioReqCount bytes from the buffer pointed to by ioBuffer and attempts to write them to the device driver having the reference number ioRefNum.
    // The drive number, if any, of the device to be written to is specified by ioVRefNum. After the write is completed, the position is returned in ioPosOffset and the number of bytes actually written is returned in ioActCount.
    OSErr err = PBWrite((ParmBlkPtr)& outgoingSerialPortReference, 0); // second param is async, TODO: investigate // returns -51:rfNumErr: refnum error
    
    if (PRINT_ERRORS) {

        char errMessage[100];
        sprintf(errMessage, "err:%d\n", err);
        printf(errMessage);
    }

    return err;
}

void setupCoprocessor(const char *name) {

    setupSerialPort(name);

    return;
}

OSErr closeSerialPort() {

    OSErr err = MacCloseDriver(outgoingSerialPortReference.ioRefNum);
    
    if (PRINT_ERRORS) {

        char errMessage[100];
        sprintf(errMessage, "err:%d\n", err);
        printf(errMessage);
    }

    return err;
}

char * _getReturnValueFromResponse(char* response, char* application_id, char* call_counter, char* operation) {

    // get the first token in to memory
    char* token = strtok(response, ";;;");

    // placeholder for the final output of the function
    char* output;
    
    // we need to track the token that we are on because the coprocessor.js responses are standardized
    // so the tokens at specific positions will map to specific items in the response
    int tokenCounter = 0;

    // loop through the string to extract all other tokens
    while (token != NULL) {

        switch (tokenCounter) {

            case 0: // APPLICATION ID

                if (strcmp(token, application_id) != 0) {

                    return "application id mismatch"; // TODO figure out better error handling
                }

                break;
            case 1: // CALL COUNTER

                if (strcmp(token, call_counter) != 0) {

                    return "call counter mismatch"; // TODO figure out better error handling
                }
 
                break;
            case 2: // OPERATION

                if (strcmp(token, operation) != 0) {
                    
                    printf("operation mismatch - operation is\n");
                    printf(operation);
                    printf("\n");
                    printf(token);
                    printf("\n^^\n");

                    return "operation mismatch"; // TODO figure out better error handling
                }

                break;
            case 3: // STATUS

                if (strcmp(token, "SUCCESS") != 0) {

                    return "operation failed"; // TODO figure out better error handling
                }

                break;
            case 4:

                output = token;

            default:

                break;
        }

        // get the next token. strtok has some weird syntax
        token = strtok(NULL, ";;;");
        tokenCounter++;
    }

    // TODO: this is a memory leak - consider providing an output variable in the parameters
    int allocationSize = 1024;
    char *output2 = NewPtr(allocationSize);
    strncpy(output2, output, allocationSize);

    return (output2);
}

void writeToCoprocessor(char* operation, char* operand) {

    const char* messageTemplate = "%s;;;%s;;;%s;;;%s;;@@&&"; // see: https://github.com/CamHenlin/coprocessor.js/blob/main/index.js#L25
    char call_id[32];

    // over-allocate by 1kb for the operand (which could be an entire nodejs app) + message template wrapper
    // and other associated info. wasting a tiny bit of memory here, could get more precise if memory becomes a problem.
    char messageToSend[strlen(operand) + 1024];

    sprintf(call_id, "%d", call_counter++);

    // application_id is globally defined for now, how will that work in a library?
    sprintf(messageToSend, messageTemplate, application_id, call_id, operation, operand);

    OSErr err = writeSerialPort(messageToSend);
    
    if (PRINT_ERRORS) {

        char errMessage[100];
        sprintf(errMessage, "writeToCoprocessor err:%d\n", err);
        printf(errMessage);
    }

    return;
}

// must be called after writeToCoprocessor and before other writeToCoprocessor
// operations because we depend on the location of call_counter
char* getReturnValueFromResponse(char *response, char *operation) {

    char *callCounterString;
    sprintf(callCounterString, "%d", (call_counter - 1));

    char *output = _getReturnValueFromResponse(response, application_id, callCounterString, operation);

    return output;
}

// TODO: this is a function we would want to expose in a library
char* sendProgramToCoprocessor(char* program) {

    writeToCoprocessor("PROGRAM", program);

    char* response = readSerialPort();
    char *returnValue = getReturnValueFromResponse(response, "PROGRAM");

    return returnValue;
}

// TODO: this is a function we would want to expose in a library
char* callFunctionOnCoprocessor(char* functionName, char* parameters) {

    const char* functionTemplate = "%s&&&%s";

    // over-allocate by 1kb for the operand (which could be whatever a programmer sends to this function) + message template wrapper
    // and other associated info. wasting a tiny bit of memory here, could get more precise if memory becomes a problem.
    char functionCallMessage[strlen(parameters) + 1024];

    // delimeter for function paramters is &&& - user must do this on their own via sprintf call or other construct - this is easiest for us to deal with
    sprintf(functionCallMessage, functionTemplate, functionName, parameters);

    writeToCoprocessor("FUNCTION", functionCallMessage);

    char* response = readSerialPort();
    char *returnValue = getReturnValueFromResponse(response, "FUNCTION");

    return returnValue;
}

// TODO: this is a function we would want to expose in a library
char* callEvalOnCoprocessor(char* toEval) {

    writeToCoprocessor("EVAL", toEval);

    char* response = readSerialPort();
    char *returnValue = getReturnValueFromResponse(response, "EVAL");

    return returnValue;
}

int main(int argc, char** argv) {

    printf("CoprocessorJS Test App: starting...\n");
    printf("CoprocessorJS Test App: setting up serial port...\n");

    setupCoprocessor("modem"); // could also be "printer", modem is 0 in PCE settings - printer would be 1

    // TODO: figure out how to move these to Mac resource files. I'm pretty sure the format is pretty easy to follow and extract from
    // started taking a stab at some of this in the Makefile
    // look at https://github.com/clehner/Browsy/blob/5c3241acd781c0555d02ed329297cb83d66bf9c2/src/uri/about.c#L76 and the corresponding makefile
    const char* TEST_INDEX = "const _ = require('lodash')\nclass SimpleNodeThing {\n  static isEmpty (variable) {\n    return _.isEmpty(variable)\n  }\n  isEmpty (variable) {\n    return this.constructor.isEmpty(variable)\n  }\n}\nmodule.exports = SimpleNodeThing";
    const char* TEST_PACKAGE = "{\n  \"name\": \"test\",\n  \"version\": \"1.0.0\",\n  \"description\": \"\",\n  \"main\": \"index.js\",\n  \"scripts\": {\n    \"test\": \"\"\n  },\n  \"author\": \"\",\n  \"license\": \"ISC\",\n  \"dependencies\": {\n    \"lodash\": \"^4.17.21\"\n  }\n}";

    char programCall[strlen(TEST_INDEX) + strlen(TEST_PACKAGE) + 1024];

    sprintf(programCall, "index.js@@@%s&&&package.json@@@%s", TEST_INDEX, TEST_PACKAGE);

    printf("CoprocessorJS Test App: attempting to send program to Coprocessor...\n");
    char* programResult = sendProgramToCoprocessor(programCall);

    printf("CoprocessorJS Test App: load program result:\n");
    printf(programResult);
    printf("\n");
    
    printf("CoprocessorJS Test App: attempting to call lodash isEmpty function with parameter \"test\" on Coprocessor...\n");
    char* jsFunctionResponse = callFunctionOnCoprocessor("isEmpty", "test");

    printf("CoprocessorJS Test App: isEmpty function call response:\n");
    printf(jsFunctionResponse);
    printf("\n");

    printf("CoprocessorJS Test App: attempting to eval code block on Coprocessor...\n");
    callEvalOnCoprocessor("console.log(this)");

    printf("\ntest complete - process any key to quit\n");
    getchar();
    closeSerialPort();

    return 0;
}
