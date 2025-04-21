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
