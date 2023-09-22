# include "BT.h"
# include "gui.h"
# include "globals.h"


wstring findSerial(BLUETOOTH_ADDRESS addr) {
    HDEVINFO hDevInfoSet = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, NULL, NULL, DIGCF_PRESENT);
    if (hDevInfoSet == INVALID_HANDLE_VALUE) {
        return L"";
    }

    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    DWORD deviceIndex = 0;

    while (SetupDiEnumDeviceInfo(hDevInfoSet, deviceIndex++, &devInfoData)) {
        WCHAR deviceID[MAX_DEVICE_ID_LEN];
        if (CM_Get_Device_IDW(devInfoData.DevInst, deviceID, MAX_DEVICE_ID_LEN, 0) != CR_SUCCESS) {
            continue;
        }

        wstring convertedAddress = std::to_wstring(addr.ullLong);
        //wcout << L"Converted Address: " << convertedAddress << L" Device ID: " << deviceID << endl;

        if (wcsstr(deviceID, L"BTHENUM\\{00001101-0000-1000-8000-00805F9B34FB}_LOCALMFG&") && wcsstr(deviceID, L"&0&98D391FE83EC_C00000000")) {
            WCHAR comPortName[100];
            DWORD regDataType;
            DWORD requiredSize;

            if (!SetupDiGetDeviceRegistryPropertyW(hDevInfoSet, &devInfoData, SPDRP_FRIENDLYNAME, &regDataType, (PBYTE)comPortName, sizeof(comPortName), &requiredSize)) {
                DWORD error = GetLastError();
                wcerr << L"Failed to get registry property for device: " << deviceID << L". Error code: " << error << endl;
            }

            //wcout << L"Friendly Name for matched address: " << comPortName << endl;

            // Extract COM Port from Friendly Name
            std::wstring friendlyName(comPortName);
            std::size_t startPos = friendlyName.find(L"(COM");
            std::size_t endPos = friendlyName.find(L")");
            if (startPos != std::wstring::npos && endPos != std::wstring::npos) {
                return friendlyName.substr(startPos + 1, endPos - startPos - 1);
            }
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfoSet);
    return L"";
}

bool initPair(bool isReconnect) {
    BLUETOOTH_FIND_RADIO_PARAMS btfrp = { sizeof(btfrp) };
    HANDLE hRadio = NULL;
    HBLUETOOTH_RADIO_FIND hFind = BluetoothFindFirstRadio(&btfrp, &hRadio);
    std::wstring comPort;

    if (hFind != NULL) {
        do {
            BLUETOOTH_RADIO_INFO bri = { sizeof(bri) };
            BluetoothGetRadioInfo(hRadio, &bri);

            BLUETOOTH_DEVICE_SEARCH_PARAMS bdsp = { sizeof(bdsp) };
            bdsp.fReturnAuthenticated = TRUE;
            bdsp.fReturnRemembered = TRUE;
            bdsp.fReturnUnknown = TRUE;
            bdsp.fReturnConnected = TRUE;
            bdsp.fIssueInquiry = TRUE;
            bdsp.cTimeoutMultiplier = 2;
            bdsp.hRadio = hRadio;

            BLUETOOTH_DEVICE_INFO bdi = { sizeof(bdi) };
            HBLUETOOTH_DEVICE_FIND hDevFind = BluetoothFindFirstDevice(&bdsp, &bdi);

            if (hDevFind != NULL) {
                do {
                    if (wcscmp(bdi.szName, L"HC-05") == 0) {
                        std::wcout << L"Found HC-05 with address: " << bdi.Address.ullLong << std::endl;

                        comPort = findSerial(bdi.Address);
                        if (!comPort.empty()) {
                            std::wcout << L"HC-05 is on: " << comPort << std::endl;
                        }
                        else {
                            std::cout << "Couldn't determine the COM port for HC-05." << std::endl;
                        }

                        BluetoothFindDeviceClose(hDevFind);
                        BluetoothFindRadioClose(hFind);
                        CloseHandle(hRadio);
                        break; // Exit the loop since we've found the HC-05
                    }
                } while (BluetoothFindNextDevice(hDevFind, &bdi));
            }
        } while (comPort.empty());
    }

    if (comPort.empty()) {
        cerr << "Couldn't determine the COM port for HC-05." << std::endl;
        return false;
    }

    g_globals.hSerial = CreateFileW(comPort.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

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

    g_globals.connected = true;
    return true;
}
bool readData() {
    chrono::time_point<chrono::high_resolution_clock> start = chrono::high_resolution_clock::now();

    while (g_globals.connected) {  // Outer loop to keep receiving data
        DWORD bytesRead;
        int16_t buffer[512] = { 0 };
        int bufferIndex = 0;
        bool inSync = false;
        while (g_globals.connected) {
            
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
                cout << "Received " << tmp << endl;

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
                    g_globals.connected = false;
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
                            g_globals.connected = true;
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
        else if (g_globals.connected){
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