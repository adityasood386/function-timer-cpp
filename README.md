# Function Timer (C++)

A Windows command-line tool that measures how long a C++ function takes to run, built around **Win32 process management**. Paste in your function and the tool compiles it, launches it as a separate process with the Windows API (`CreateProcess`), waits on it with a timeout (`WaitForSingleObject`), and force-terminates it (`TerminateProcess`) if it hangs, for example in an infinite loop. The execution time is reported in milliseconds. I am still learning :)

## Features

- Uses the **Win32 API** to launch user code as a separate child process, so a crash or hang does not take down the tool
- Enforces a **3-second timeout** with `WaitForSingleObject` and kills runaway processes with `TerminateProcess`
- Cleans up process and thread handles with `CloseHandle` to avoid resource leaks
- Paste any C++ function directly into the terminal, with no need to edit a source file
- Automatically wraps your code with a timing harness built on `std::chrono::high_resolution_clock`
- Reports execution time in milliseconds

## How It Works

1. You enter the name of the function you want to time.
2. You paste your C++ code and finish with `Ctrl+Z`, then `Enter` (Windows end-of-input).
3. The tool generates `usercode.cpp`, which contains the required headers, your code, and a `main()` that:
   - records the start time
   - calls your function
   - records the end time and prints the elapsed milliseconds
4. The generated file is compiled with `g++` into `usercode.exe`.
5. The tool launches `usercode.exe` using the Windows API (`CreateProcess`) and waits up to 3000 ms. If the process is still running after that, it is terminated with `TerminateProcess`.

## Windows Programming Concepts

The process-control part of the tool is written directly against the Windows API (`<windows.h>`):

| API | Role in this project |
|-----|----------------------|
| `STARTUPINFO` | Describes how the new process should start; initialised with its own size, as the API requires |
| `PROCESS_INFORMATION` | Receives the handles and IDs of the new process and its main thread |
| `CreateProcess` | Launches the compiled `usercode.exe` as an independent child process |
| `WaitForSingleObject` | Blocks until the process exits or the timeout (3000 ms) expires |
| `WAIT_TIMEOUT` | Return value used to detect that the process ran too long |
| `TerminateProcess` | Forcibly stops a process that exceeded the timeout |
| `CloseHandle` | Releases the process and thread handles on every exit path |

Running the user's code in its own process, instead of calling it inside the tool, is what makes the timeout possible. An infinite loop in the same process would freeze the tool itself, but a separate process can be killed from outside.

## Requirements

- Windows (the tool uses `windows.h` for process creation)
- A C++ compiler available on your `PATH` as `g++` (for example, [MinGW-w64](https://www.mingw-w64.org/) or MSYS2)

## Build

```bash
g++ function_timer.cpp -o function_timer.exe
```

## Usage

Run the tool:

```bash
function_timer.exe
```

Then follow the prompts:

```
Enter function name :
abc
Paste your C++ code below (press Ctrl+Z then Enter to finish):
void abc() {
    long long sum = 0;
    for (int i = 0; i < 100000000; i++) sum += i;
    cout << sum << endl;
}
^Z
```

Example output (the timing will vary by machine):

```
4999999950000000
Your function took : 187 ms
User code ran successfully.
```

### Timeout example

If your function never finishes, the tool stops it after 3 seconds:

```cpp
void abc() {
    while (true) {
        cout << "hello" << endl;
    }
}
```

```
Process killed due to timeout (could be infinite loop).
User code terminated.
```

## Input Rules

- Paste a **complete function definition**, not loose statements. Statements outside a function (such as a bare `while` loop) will fail to compile.
- The function must take **no arguments**, because it is called as `function_name();`.
- Do not include your own `main()`. The tool generates one.
- `<iostream>`, `<chrono>`, and `<thread>` are already included and `using namespace std;` is already applied. Add any other `#include` lines at the top of your pasted code.
- The function name is read as a single word, so it cannot contain spaces.

## Project Structure

```
function-timer-cpp/
├── function_timer.cpp   # the tool (main program)
├── README.md
```

`usercode.cpp` and `usercode.exe` are generated every time you run the tool, so they are not stored in the repository.

## Known Limitations

- Windows only
- Only zero-argument functions can be timed
- Results are reported in whole milliseconds, so very fast functions may show `0 ms`
- The time includes anything the function prints to the console
- The compiler's exit status is not checked, so if your code fails to compile, a previously built `usercode.exe` may run instead of showing an error

## Future Improvements

- Check the `g++` result and stop with a clear message when compilation fails
- Report time in microseconds or nanoseconds for fast functions
- Run the function multiple times and report average, minimum, and maximum
- Support functions that take arguments
- Make the timeout configurable
- Add Linux and macOS support (replacing `CreateProcess` with a cross-platform approach)

## Author

Aditya, B.Tech CSE student at GNDU Regional Campus, Jalandhar.
GitHub: [adityasood386](https://github.com/adityasood386)

## License

This project is released under the MIT License. Add a `LICENSE` file to the repository if you want to use it.
