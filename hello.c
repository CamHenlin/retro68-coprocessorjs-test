#include <Resources.h>
#include <stdio.h>
#include "output_js.h"
#include "coprocessorjs.h"

int main(int argc, char** argv) {

    printf("CoprocessorJS Test App: starting...\n");
    printf("CoprocessorJS Test App: setting up serial port...\n");

    setupCoprocessor("my_application_id", "modem"); // could also be "printer", modem is 0 in PCE settings - printer would be 1

    printf("CoprocessorJS Test App: attempting to send program to Coprocessor...\n");
    char programResult[MAX_RECEIVE_SIZE];
    sendProgramToCoprocessor(OUTPUT_JS, programResult);

    printf("CoprocessorJS Test App: load program result:\n");
    printf(programResult);
    
    printf("\nCoprocessorJS Test App: attempting to call puppeteer wrapper function 'getBodyAtURL' on Coprocessor...\n");
    char jsFunctionResponse[MAX_RECEIVE_SIZE];
    callFunctionOnCoprocessor("getBodyAtURL", "https://68kmla.org/forums/topic/62382-serial-based-remote-code-execution-idea-for-enabling-modern-software-on-classic-macs/", jsFunctionResponse);

    printf("CoprocessorJS Test App: getBodyAtURL function call response:\n");
    printf(jsFunctionResponse);
    printf("\n");

//    printf("CoprocessorJS Test App: attempting to eval code block on Coprocessor...\n");
//    callEvalOnCoprocessor("console.log(this)");

    printf("\ntest complete - process any key to quit\n");
    getchar();
    closeSerialPort();

    return 0;
}
