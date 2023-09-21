# include "BT.h"
# include "gui.h"
# include "globals.h"


// https://stackoverflow.com/a/56959139, modified
string findSerial() {
    char lpTargetPath[5000]; // buffer to store the path of the COMPORTS

    for (int i = 0; i < 255; i++) // checking ports from COM0 to COM255
    {
        string port = "COM" + to_string(i); // converting to COM0, COM1, COM2
        DWORD test = QueryDosDevice(port.c_str(), lpTargetPath, 5000);

        if (test != 0) //QueryDosDevice returns zero if it didn't find an object
        {
            string path = lpTargetPath;  // convert to str
            size_t last = path.find_last_of('\\');  // find end of path

            if (last != string::npos) {
                string deviceName= path.substr(last + 1);

                if (deviceName.compare("HC-05") == 0) {     // TODO: test what the actual device name of the HC-05 is and cross test on other devices!!
                    return port;
                }
            }
        }
    }

    return "";
}

bool initPair(bool isReconnect) {
    string port = findSerial();

    while (port.compare("HC-05") != 0) {
        cout << "Failed to find HC-05 port" << endl;
        port = findSerial();
        Sleep(1000);
    }

    g_globals.hSerial = CreateFile(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);  // TODO: see if we can list all valid COMM ports for user to select

    if (g_globals.hSerial == INVALID_HANDLE_VALUE) {
        if (!isReconnect) {
            cerr << "Failed connection to selected COM port." << endl;
        }
        return false;
    }

    DCB serialParams = {};
    serialParams.DCBlength = sizeof(serialParams);

    if (!GetCommState(g_globals.hSerial, &serialParams)) {
        cerr << "Current serial parameters could not be found." << endl;
        return false;
    }

    serialParams.BaudRate = 9600;
    serialParams.ByteSize = 8;
    serialParams.StopBits = ONESTOPBIT;
    serialParams.Parity = NOPARITY;

    if (!SetCommState(g_globals.hSerial, &serialParams)) {
        cerr << "Current serial parameters could not be set." << endl;
        return false;
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50; // max time between read char in ms
    timeouts.ReadTotalTimeoutConstant = 50; // total time read can take in ms
    timeouts.ReadTotalTimeoutMultiplier = 10; // time per each byte read
    timeouts.WriteTotalTimeoutConstant = 50; // total time write can take in ms
    timeouts.WriteTotalTimeoutMultiplier = 10; // time per each byte written

    if (!SetCommTimeouts(g_globals.hSerial, &timeouts)) {
        cerr << "Error setting timeouts." << endl;
        return false;
    }

    return true;
}
bool readData() {
    chrono::time_point<chrono::high_resolution_clock> start = chrono::high_resolution_clock::now();

    while (true) {  // Outer loop to keep receiving data
        DWORD bytesRead;
        int16_t buffer[512] = { 0 };
        int bufferIndex = 0;
        bool inSync = false;
        while (true) {
            
            int16_t tmp;

            if (!ReadFile(g_globals.hSerial, &tmp, sizeof(int16_t), &bytesRead, NULL)) {
                cerr << "Failed to read from COM port. Error code: " << GetLastError() << endl;
                Sleep(100);  // sleep a bit before next loop iteration
                continue;  // return to the beginning of the loop
            }

            if (bytesRead == sizeof(int16_t)) {

                if (tmp == 0 && !inSync) {
                    continue; // Ignore null values
                }

                start = chrono::high_resolution_clock::now();
                //cout << "Received " << tmp << endl;

                if (tmp == SOM_MARKER) {
                    // Found the beginning of the message, set the inSync flag
                    inSync = true;
                    bufferIndex = 0;
                    continue;
                }

                if (tmp == EOM_MARKER) {
                    // Found the end of the message, break to process the buffer
                    if (inSync) {
                        break;
                    }
                    else {
                        continue;
                        cerr << "Out of sync!" << endl;
                        return false;
                    }
                }

                if (inSync) {
                    buffer[bufferIndex] = tmp;
                    bufferIndex++;
                    if (bufferIndex >= sizeof(buffer) / sizeof(int16_t)) {
                        cerr << "Buffer overflow. Buffer size: " << sizeof(buffer) / sizeof(int16_t) << ", Current index: " << bufferIndex << endl;
                        return false;
                    }
                }
                else {
                    // Skip any bytes until we find an SOM marker to sync up with
                    continue;
                }
            }
            else { //no message received
                chrono::time_point<chrono::high_resolution_clock> end = chrono::high_resolution_clock::now();
                if (chrono::duration_cast<chrono::seconds>(end - start).count() >= 2) {
                    cerr << "Connection lost." << endl;
                    int retryCount = 0;
                    const int maxRetries = 5;

                    if (g_globals.hSerial != INVALID_HANDLE_VALUE) {
                        if (!CloseHandle(g_globals.hSerial)) {
                            cerr << "Failed to close the handle. Error code: " << GetLastError() << endl;
                        }
                        g_globals.hSerial = INVALID_HANDLE_VALUE;  // Set it to an invalid value after closing
                    }

                    while (retryCount < maxRetries) {
                        cerr << "Attempting to reconnect... (Attempt " << (retryCount + 1) << ")" << endl;
                        if (initPair(true)) {
                            cerr << "Reconnected successfully." << endl;
                            start = chrono::high_resolution_clock::now();
                            break;
                        }
                        else {
                            cerr << "Failed to reconnect. Waiting before next attempt." << endl;
                            Sleep(10);
                            retryCount++;
                        }
                    }

                    if (retryCount == maxRetries) {
                        cerr << "Max retry attempts reached. Exiting." << endl;
                        return false;
                    }
                }
            }
        }

        if (inSync) {
            // Process the buffer now
            for (int i = 0; i < bufferIndex; ++i) {  // acount for 

                if (i == 300) {
                    g_globals.tempIndex = buffer[i];
                }
                else {
                    g_globals.tempData.push_back((float)buffer[i] / 10.f);
                }

                if (g_globals.tempData.size() > 300) {
                    g_globals.tempData.erase(g_globals.tempData.begin());
                }
            }



           // cout << bufferIndex << endl;
           cout << "Last Temp: " << g_globals.tempData.back() << endl;

        }
        else {
            cerr << "SOM not found. Invalid data stream." << endl;
        }
    }

    return true;
}

bool writeData() {
    CommandData cmd;

    switch (g_globals.currentID) {
    case globals::MessageID::NA:
        return false;
    case globals::MessageID::setDisplay:
        cmd.id = globals::MessageID::setDisplay;
        cmd.data = static_cast<int16_t>(g_globals.enableLED);
        break;
    }

    DWORD bytesWritten;

    // SEND SOM
    if (!WriteFile(g_globals.hSerial, &SOM_MARKER, sizeof(SOM_MARKER), &bytesWritten, NULL)) {
        cerr << "Error writing SOM to port." << endl;
        return false;
    }

    // SEND DATA
    if (!WriteFile(g_globals.hSerial, &cmd, sizeof(CommandData), &bytesWritten, NULL)) {
        cerr << "Error writing CommandData to the port." << endl;
        return false;
    }

    // SEND EOM
    if (!WriteFile(g_globals.hSerial, &EOM_MARKER, sizeof(EOM_MARKER), &bytesWritten, NULL)) {
        cerr << "Error writing EOM to port." << endl;
        return false;
    }

    // reset to default ID value
    g_globals.currentID = globals::MessageID::NA;
   
    return true;
}