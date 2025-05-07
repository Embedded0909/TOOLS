Call exe from exe
```cpp
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

void ParseHexToArray(const std::string& output, int arr[], int maxCount) {
    std::istringstream iss(output);
    for (int i = 0; i < maxCount; ++i) {
        if (!(iss >> std::hex >> arr[i])) {
            arr[i] = 0; 
        }
    }
}

std::string RunExeWithInput(const std::string& exePath, const std::string& input) {
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE hChildStd_IN_Rd = NULL;
    HANDLE hChildStd_IN_Wr = NULL;
    HANDLE hChildStd_OUT_Rd = NULL;
    HANDLE hChildStd_OUT_Wr = NULL;

    CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &sa, 0);
    CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &sa, 0);
    SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0);

    PROCESS_INFORMATION pi;
    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    si.hStdError = hChildStd_OUT_Wr;
    si.hStdOutput = hChildStd_OUT_Wr;
    si.hStdInput = hChildStd_IN_Rd;
    si.dwFlags |= STARTF_USESTDHANDLES;

    if (!CreateProcessA(NULL, const_cast<char*>(exePath.c_str()), NULL, NULL, TRUE,
        0, NULL, NULL, &si, &pi)) {
        std::cerr << "CreateProcess failed." << std::endl;
        return "";
    }
    CloseHandle(hChildStd_OUT_Wr);
 
    DWORD written;
    WriteFile(hChildStd_IN_Wr, input.c_str(), input.length(), &written, NULL);
    CloseHandle(hChildStd_IN_Wr); 


    char buffer[128];
    DWORD read;
    std::string output;
    while (ReadFile(hChildStd_OUT_Rd, buffer, sizeof(buffer), &read, NULL) && read > 0) {
        output.append(buffer, read);
    }

 
    CloseHandle(hChildStd_OUT_Rd);
    CloseHandle(hChildStd_IN_Rd);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return output;
}

int main() {
    std::string result = RunExeWithInput("core/test01.exe", "12 34\n");
    std::cout << "Raw output: " << result << std::endl;

    const int MAX_NUM = 12;
    int arr[MAX_NUM];

    ParseHexToArray(result, arr, MAX_NUM);

    std::cout << "Parsed hex values (decimal view): ";
    for (int i = 0; i < MAX_NUM; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}

```

update 
```cpp
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stdint.h>

void ParseHexToArray(const std::string& output, int arr[], int maxCount) {
    std::istringstream iss(output);
    for (int i = 0; i < maxCount; ++i) {
        if (!(iss >> std::hex >> arr[i])) {
            arr[i] = 0;
        }
    }
}

std::string TrimFromHex(const std::string& input, const std::string& startPattern, size_t length) {
    size_t pos = input.find(startPattern);
    if (pos == std::string::npos) return ""; 
    return input.substr(pos, length);
}

std::string RunExeWithInput(const std::string& exePath, const std::string& input) {
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE hChildStd_IN_Rd = NULL;
    HANDLE hChildStd_IN_Wr = NULL;
    HANDLE hChildStd_OUT_Rd = NULL;
    HANDLE hChildStd_OUT_Wr = NULL;

    CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &sa, 0);
    CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &sa, 0);
    SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0);

    PROCESS_INFORMATION pi;
    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    si.hStdError = hChildStd_OUT_Wr;
    si.hStdOutput = hChildStd_OUT_Wr;
    si.hStdInput = hChildStd_IN_Rd;
    si.dwFlags |= STARTF_USESTDHANDLES;

    if (!CreateProcessA(NULL, const_cast<char*>(exePath.c_str()), NULL, NULL, TRUE,
        0, NULL, NULL, &si, &pi)) {
        std::cerr << "CreateProcess failed." << std::endl;
        return "";
    }
    CloseHandle(hChildStd_OUT_Wr);

    DWORD written;
    WriteFile(hChildStd_IN_Wr, input.c_str(), input.length(), &written, NULL);
    CloseHandle(hChildStd_IN_Wr);


    char buffer[256];
    Sleep(90);
    DWORD read;
    //std::string output;
    std::string output;
    std::string trimmed;
    while (ReadFile(hChildStd_OUT_Rd, buffer, sizeof(buffer), &read, NULL) && read > 0) {
        output.append(buffer, read);
        //Sleep(10);
        break;
    }
    trimmed = TrimFromHex(output, "43 58", 50);

    CloseHandle(hChildStd_OUT_Rd);
    CloseHandle(hChildStd_IN_Rd);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    TerminateProcess(pi.hProcess, 0);
    return trimmed;
}

int main() {
    std::string result = RunExeWithInput("core/Sabi_v1.5.exe", "7A 82 83");
    std::cout << "Raw output: " << result << std::endl;

    const int MAX_NUM = 12;
    int arr[MAX_NUM];

    ParseHexToArray(result, arr, MAX_NUM);

    std::cout << "Parsed hex values (decimal view): ";
    for (int i = 0; i < MAX_NUM; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}

```

update 
```cpp
#define _CRT_SECURE_NO_DEPRECATE
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

//---------------------------------------------VAR-----------------------------------------

extern "C" {
    typedef int(__stdcall* MYPROC) (UCHAR i[], UCHAR o[]);
}

typedef struct {
    uint16_t time;
    uint8_t type;
    uint8_t data;
} Log_typedef;


typedef struct {
    uint8_t header[4];
    uint8_t cnt;
    uint8_t mode;
    Log_typedef log[16];
} EClogging_typedef;


//EClogging_typedef mdata;


EClogging_typedef mdata = {
    {0xFF, 0xFF, 0xFF, 0xFF},
    0xFF,
    0xFF,
    {
        {0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF}, {0xFF, 0xFF, 0xFF}
    }
};



uint8_t* fillDataPtr = &(mdata.header[0]);

HWND hTextBox;

int outputBuff[31];

MYPROC mFunction;


std::string strInput[7] = { "7A 82 80 EE 00", "7A 82 80 EE 01", "7A 82 80 EE 02", "7A 82 80 EE 03", "7A 82 80 EE 04", "7A 82 80 EE 05","7A 82 80 EE 06" };

uint8_t bugCheck[31];

//--------------------------------------------FUNCTION----------------------------------------

void ParseHexToArray(const std::string& output, int arr[], int maxCount) {
    std::istringstream iss(output);
    for (int i = 0; i < maxCount; ++i) {
        if (!(iss >> std::hex >> arr[i])) {
            arr[i] = 0;
        }
    }
}

std::string TrimFromHex(const std::string& input, const std::string& startPattern, size_t length) {
    size_t pos = input.find(startPattern);
    if (pos == std::string::npos) return "";
    return input.substr(pos, length);
}
std::string output;
std::string trimmed;

std::string RunExeWithInput(const std::string& exePath, const std::string& input) {
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE hChildStd_IN_Rd = NULL;
    HANDLE hChildStd_IN_Wr = NULL;
    HANDLE hChildStd_OUT_Rd = NULL;
    HANDLE hChildStd_OUT_Wr = NULL;

    CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &sa, 0);
    CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &sa, 0);
    SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0);

    PROCESS_INFORMATION pi;
    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    si.hStdError = hChildStd_OUT_Wr;
    si.hStdOutput = hChildStd_OUT_Wr;
    si.hStdInput = hChildStd_IN_Rd;
    si.dwFlags |= STARTF_USESTDHANDLES;

    if (!CreateProcessA(NULL, const_cast<char*>(exePath.c_str()), NULL, NULL, TRUE,
        0, NULL, NULL, &si, &pi)) {
        std::cerr << "CreateProcess failed." << std::endl;
        return "";
    }
    CloseHandle(hChildStd_OUT_Wr);

    DWORD written;
    WriteFile(hChildStd_IN_Wr, input.c_str(), input.length(), &written, NULL);
    CloseHandle(hChildStd_IN_Wr);


    char buffer[256];
    Sleep(500);
    DWORD read;
    //std::string output;
    //std::string output;
    //std::string trimmed;
    while (ReadFile(hChildStd_OUT_Rd, buffer, sizeof(buffer), &read, NULL) && read > 0) {
        output.append(buffer, read);
        break;
    }
    trimmed = TrimFromHex(output, "43 58", 50);

    CloseHandle(hChildStd_OUT_Rd);
    CloseHandle(hChildStd_IN_Rd);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    TerminateProcess(pi.hProcess, 0);
    return trimmed;
}



void resetOutput() {
    for (int i = 0; i < 31;i++) {
        outputBuff[i] = 0;
    }
}


void saveToFileTxt(const char* filename, const EClogging_typedef* data) {
    FILE* file = NULL;

    if (fopen_s(&file, filename, "w") != 0) {
        return;
    }

    fprintf(file, "Header: 0x%02X", data->header[0]);
    fprintf(file, "%02X", data->header[1]);
    fprintf(file, "%02X", data->header[2]);
    fprintf(file, "%02X\n", data->header[3]);
    fprintf(file, "Cnt: 0x%02X\n", data->cnt);
    fprintf(file, "Mode: 0x%02X\n", data->mode);
    fprintf(file, "[\n");

    for (int i = 0; i < 16; i++) {
        fprintf(file, "   {\n");
        fprintf(file, "          Time: 0x%04X\n", data->log[i].time);
        fprintf(file, "          Type: 0x%02X\n", data->log[i].type);
        fprintf(file, "          Data: 0x%02X\n", data->log[i].data);
        fprintf(file, "   }\n");
    }
    fprintf(file, "]\n");
    fclose(file);

}
int check = 0;

void callSabiFunction() {
   
    for (int i = 0; i < 7; i++) {
        resetOutput();
        std::string result = RunExeWithInput("core/Sabi_v1.5.exe", strInput[i] );
        ParseHexToArray(result, outputBuff, 31);
        for (int j = 7; j < 17;j++) {
            *fillDataPtr = (uint8_t)outputBuff[j];
            fillDataPtr++;

        }
    }

}

int handle() {
    uint8_t status = 0;
    callSabiFunction();
    return status;
};

void convertTime(uint16_t time, uint8_t *hour, uint8_t *minute, uint8_t *second) {
     *hour = (time >> 10) & 0x0F;
     *minute = (time >> 4) & 0x3F;
     *second = time & 0x3F;
}

void DisplayLog(HWND hwnd) {
    char buffer[2048];
    int len = 0;
    uint8_t hour, minute, second;
    
    len += sprintf(buffer + len, "Header: 0x%02X%02X%02X%02X\r\n",
        mdata.header[0], mdata.header[1], mdata.header[2], mdata.header[3]);
    len += sprintf(buffer + len, "Cnt: 0x%02X\r\n", mdata.cnt);
    len += sprintf(buffer + len, "Mode: 0x%02X\r\n", mdata.mode);
    len += sprintf(buffer + len, "[\r\n");

    for (int i = 0; i < 16; i++) {
        convertTime(mdata.log[i].time, &hour, &minute, &second);
        //convertTime(1972296+i, &hour, &minute, &second);

        len += sprintf(buffer + len, "      {\r\n");
        len += sprintf(buffer + len, "         Time: 0x%04X      (%dh %dm %ds)\r\n", mdata.log[i].time , hour, minute, second);
        len += sprintf(buffer + len, "         Type: 0x%02X\r\n", mdata.log[i].type);
        len += sprintf(buffer + len, "         Data: 0x%02X\r\n", mdata.log[i].data);
        len += sprintf(buffer + len, "      }\r\n");
    }
    len += sprintf(buffer + len, "]\r\n");
    len += sprintf(buffer + len, "Check = %d", check );
    len += sprintf(buffer + len, "bug[0] = %02X", bugCheck[0]);
    len += sprintf(buffer + len, "bug[1] = %02X", bugCheck[1]);
    len += sprintf(buffer + len, "bug[2] = %02X", bugCheck[2]);
    len += sprintf(buffer + len, "bug[3] = %02X", bugCheck[3]);
    len += sprintf(buffer + len, "bug[4] = %02X", bugCheck[4]);
    len += sprintf(buffer + len, "bug[5] = %02X", bugCheck[5]);
    len += sprintf(buffer + len, "bug[6] = %02X", bugCheck[6]);
    len += sprintf(buffer + len, "bug[7] = %02X", bugCheck[7]);


    wchar_t wbuffer[2048];
    MultiByteToWideChar(CP_UTF8, 0, buffer, -1, wbuffer, 2048);

    SetWindowText(hTextBox, wbuffer);
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {

        CreateWindow(L"BUTTON", L"GetLog",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            10, 10, 100, 30, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);


        hTextBox = CreateWindow(L"EDIT", L"",
            WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | WS_VSCROLL,
            120, 10, 600, 400, hwnd, (HMENU)2, GetModuleHandle(NULL), NULL);
        break;
    }
    case WM_COMMAND: {

        if (LOWORD(wParam) == 1) {
            uint8_t status = handle();
            if (status == 0) {
                saveToFileTxt("LOG_MICOM_SHUTDOWN.txt", &mdata);
                DisplayLog(hwnd);
            }
            else if(status == 1){
                SetWindowText(hTextBox, L"Err = 1\r\n");
            }
            else if (status == 2) {
                SetWindowText(hTextBox, L"Err = 2\r\n");
            }
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"LogViewerWindowClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, L"Log EC RAM",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        800, 500, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

```
update 
```
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Upload Ảnh BMP</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f2f2f2;
            text-align: center;
            padding-top: 50px;
        }

        form {
            background-color: #fff;
            padding: 20px;
            border-radius: 10px;
            display: inline-block;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
        }

        input[type="file"] {
            margin: 10px 0;
        }

        img {
            margin-top: 20px;
            max-width: 300px;
            border: 1px solid #ccc;
        }
    </style>
</head>
<body>
    <h3>Upload ảnh BMP</h3>
    <form method="POST" action="/upload" enctype="multipart/form-data">
        <input type="file" name="image" accept=".bmp">
        <br>
        <input type="submit" value="Tải lên">
    </form>

    <!-- Ảnh sẽ hiển thị nếu đường dẫn hợp lệ -->
   
</body>
</html>

```
