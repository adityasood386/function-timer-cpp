# Function Timer (C++)

A small Windows command-line tool that measures how long a C++ function takes to run. Paste in your function, tell the tool its name, and it compiles and runs your code in a separate process and reports the execution time in milliseconds. If the code runs longer than 3 seconds (for example, an infinite loop), the tool kills it automatically.

## Features

- Paste any C++ function directly into the terminal, with no need to edit a source file
- Automatically wraps your code with a timing harness built on `std::chrono::high_resolution_clock`
- Compiles and runs your code as a separate process, so a crash or hang does not take down the tool
- 3-second timeout that terminates infinite loops and very slow code
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
└── .gitignore
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
