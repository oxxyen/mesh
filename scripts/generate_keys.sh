#!/bin/bash

echo "🔑 Generating SSL keys for Secure Messenger..."

mkdir -p config/ssl

# Generate CA private key
openssl genrsa -out config/ssl/ca-key.pem 2048

# Generate CA certificate
openssl req -new -x509 -nodes -days 3650 \
    -key config/ssl/ca-key.pem \
    -out config/ssl/ca-cert.pem \
    -subj "/C=US/ST=State/L=City/O=SecureMessenger/CN=CA"

# Generate server key
openssl genrsa -out config/ssl/server-key.pem 2048

# Generate server certificate request
openssl req -new -key config/ssl/server-key.pem \
    -out config/ssl/server.csr \
    -subj "/C=US/ST=State/L=City/O=SecureMessenger/CN=server"

# Sign server certificate
openssl x509 -req -in config/ssl/server.csr \
    -CA config/ssl/ca-cert.pem \
    -CAkey config/ssl/ca-key.pem \
    -CAcreateserial \
    -out config/ssl/server-cert.pem \
    -days 3650

echo "✅ SSL keys generated in config/ssl/"
