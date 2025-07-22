#!/bin/bash
# Build script for ESP32 firmware

set -e  # Exit on any error

echo "🔧 Building ESP32 firmware..."

# Get ESP-IDF environment
echo "Setting up ESP-IDF environment..."
if [ -f "/Users/j/esp/esp-idf/export.sh" ]; then
    source "/Users/j/esp/esp-idf/export.sh"
else
    echo "ERROR: ESP-IDF not found at /Users/j/esp/esp-idf/export.sh"
    exit 1
fi

# Change to firmware directory (parent of scripts)
cd "$(dirname "$0")/.."

# Clean old build directory
if [ -d "build" ]; then
    echo "Removing old build directory..."
    rm -rf build
fi

# Load environment variables
echo "Loading WiFi credentials..."
source ./scripts/setenv.sh

# Build the project
echo "Building project..."
idf.py build

echo "✅ Build completed successfully!"
echo "Run './flash.sh' to flash to device"