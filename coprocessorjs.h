

const int MAX_RECEIVE_SIZE;

void setupCoprocessor(const char *application_id, const char *serialDeviceName);

void sendProgramToCoprocessor(char* program, char *output);

void callFunctionOnCoprocessor(char* functionName, char* parameters, char* output);

void callEvalOnCoprocessor(char* toEval, char* output);

OSErr closeSerialPort();