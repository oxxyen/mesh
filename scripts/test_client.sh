#!/bin/bash

echo "🧪 Testing Secure Messenger Client..."

# Build the project
echo "🔨 Building client..."
make client

if [ $? -ne 0 ]; then
    echo "❌ Build failed"
    exit 1
fi

echo "🚀 Starting client..."
echo "🔗 Connecting to 127.0.0.1:8888"
echo ""

./bin/messenger_client 127.0.0.1 8888
