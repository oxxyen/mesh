#!/bin/bash

echo "🔨 Building Secure Messenger..."

# Create directories
mkdir -p bin lib obj

# Build the project
make all

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "📦 Binaries created in ./bin/"
    echo "   - messenger_server"
    echo "   - messenger_client"
else
    echo "❌ Build failed!"
    exit 1
fi
