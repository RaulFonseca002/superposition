#!/bin/bash

# This script will exit immediately if any command fails.
set -e

# --- Commands ---

# Configures the project using CMake. Creates the 'build' directory.
configure() {
    echo "--- Configuring Project ---"
    cmake -B build
}

# Builds the project using the existing configuration.
build() {
    echo "--- Building Project ---"
    cmake --build build
}

# Runs the main Superposition executable.
run() {
    echo "--- Building and Running Project ---"
    build # Ensure the project is built before running
    ./build/Superposition
}

# Runs the unit tests.
test() {
    echo "--- Building and Running Tests ---"
    build # Ensure the tests are built before running
    ctest --test-dir build --output-on-failure
}

# Deletes the build directory for a clean start.
clean() {
    echo "--- Cleaning Build Directory ---"
    rm -rf build
}

# Prints the help message.
usage() {
    echo "Usage: $0 {configure|build|run|test|clean}"
    echo "  configure: Sets up the build directory for the first time."
    echo "  build: Compiles the project."
    echo "  run: Builds and runs the main executable."
    echo "  test: Builds and runs the unit tests."
    echo "  clean: Deletes the build directory."
}

# --- Main Logic ---

# Check if a command was passed
if [ -z "$1" ]; then
    echo "Error: No command specified."
    usage
    exit 1
fi

# Execute the command passed as the first argument
case "$1" in
    configure)
        configure
        ;;
    build)
        build
        ;;
    run)
        run
        ;;
    test)
        test
        ;;
    clean)
        clean
        ;;
    *)
        echo "Error: Unknown command '$1'"
        usage
        exit 1
        ;;
esac