#include <iostream>
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>
using namespace std;

string usercode;
string func_name;

string getUserCode()
{
    string line;
    ostringstream code;

    cout << "Enter function name :" << endl;
    cin >> func_name;
    cin.ignore();

    cout << "Paste your C++ code below (press Ctrl+Z then Enter to finish):\n";

    while (getline(cin, line))
    {
        code << line << '\n';
    }

    return code.str();
}

bool runWithTimeout(const char* exeName, DWORD timeout_ms)
{
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (CreateProcess(
                NULL,
                (LPSTR)exeName,
                NULL,
                NULL,
                FALSE,
                0,
                NULL,
                NULL,
                &si,
                &pi))
    {
        DWORD waitResult = WaitForSingleObject(pi.hProcess, timeout_ms);

        if (waitResult == WAIT_TIMEOUT)
        {
            TerminateProcess(pi.hProcess, 1);
            cout << "Process killed due to timeout (could be infinite loop).\n";
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return false;
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return true;
    }
    else
    {
        cout << "Failed to start process.\n";
        return false;
    }
}

int main()
{
    fstream fout;
    fout.open("usercode.cpp", ios::out);
    usercode = getUserCode();

    fout << "#include <iostream>\n";
    fout << "#include <chrono>\n";
    fout << "#include <thread>\n";
    fout << "using namespace std;\n";
    fout << usercode << "\n";
    fout << "int main() {\n";
    fout << "    auto start = std::chrono::high_resolution_clock::now();\n\n";
    fout << "    " << func_name << "();\n";
    fout << "    auto en = std::chrono::high_resolution_clock::now();\n";
    fout << "    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(en - start);\n";
    fout << "    cout << \"Your function took : \" << duration.count() << \" ms\" << endl;\n";
    fout << "    return 0;\n";
    fout << "}\n";

    fout.close();

    system("g++ usercode.cpp -o usercode.exe");

    bool success = runWithTimeout("usercode.exe", 3000);

    if (success)
    {
        cout << "User code ran successfully.\n";
    }
    else
    {
        cout << "User code terminated.\n";
    }


}
