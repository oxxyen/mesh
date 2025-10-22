#!/bin/bash

echo "🧪 Testing Secure Messenger Server..."

# Check if Redis is running
if ! redis-cli ping &> /dev/null; then
    echo "❌ Redis is not running. Starting Redis..."
    sudo systemctl start redis-server
    sleep 2
fi

# Build the project
echo "🔨 Building server..."
make server

if [ $? -ne 0 ]; then
    echo "❌ Build failed"
    exit 1
fi

echo "🚀 Starting server..."
echo "📍 Server will run on 127.0.0.1:8888"
echo "⏹️  Press Ctrl+C to stop the server"
echo ""

./bin/messenger_server
