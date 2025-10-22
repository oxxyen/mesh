#!/bin/bash

echo "🔧 Setting up Redis for Secure Messenger..."

# Check if Redis is installed
if ! command -v redis-server &> /dev/null; then
    echo "📥 Installing Redis..."
    sudo apt-get update
    sudo apt-get install -y redis-server
fi

# Start Redis service
echo "🚀 Starting Redis service..."
sudo systemctl start redis-server
sudo systemctl enable redis-server

# Test Redis connection
echo "🔍 Testing Redis connection..."
if redis-cli ping | grep -q "PONG"; then
    echo "✅ Redis is working correctly"
else
    echo "❌ Redis connection failed"
    exit 1
fi

echo "✅ Redis setup complete!"
echo "📊 Redis is running on port 6379"
