# pipo_cpp_project
HSE PIPO course project repo 

## Project Structure

- `src/` - Source files
  - `main.cpp` - Main application entry point
- `include/` - Header files
- `tests/` - Test files using Google Test
  - `test_main.cpp` - Test suite

## Build Instructions

### Requirements
- CMake 3.14 or higher
- C++20 compatible compiler (GCC, Clang, MSVC)

### Building the Project

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build
cmake --build .
```

### Running the Application

```bash
# From build directory
./pipo-hse           # Linux/macOS
.\pipo-hse.exe       # Windows
```

### Running Tests

```bash
# From build directory
ctest --output-on-failure

# Or run test executable directly
./tests_run          # Linux/macOS
.\tests_run.exe      # Windows
```


