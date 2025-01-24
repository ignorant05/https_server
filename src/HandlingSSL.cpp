//
// Created by ignorant05 on 1/22/25.
//

#include "HandlingSSL.h"

#include <openssl/err.h>
#include <openssl/ssl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int port = 4433;
;
const int BUFFER_SIZE = 1024;

void HandlingSSL::Initialize() {
    OpenSSL_add_ssl_algorithms();
    SSL_load_error_strings();
}

SSL_CTX* HandlingSSL::CreateContext() {
    return SSL_CTX_new(TLS_server_method());
}

void HandlingSSL::ConfigureContext(SSL_CTX *ctx) {
    if (SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION) != 1) {
        ERR_print_errors_fp(stderr);
        abort();
    }
    if (SSL_CTX_use_certificate_file(ctx, "/required/cert.pem", SSL_FILETYPE_PEM) < 0) {
        ERR_print_errors_fp(stderr);
        abort();
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, "/required/key.pem", SSL_FILETYPE_PEM) < 0) {
        ERR_print_errors_fp(stderr);
        abort();
    }
    if (SSL_CTX_set_cipher_list(ctx, "HIGH:!aNULL:!MD5:!RC4:!3DES") < 0) {
        ERR_print_errors_fp(stderr);
        abort();
    }
}

int HandlingSSL::CreateSocket() {
    int ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(ServerSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (ServerSocket <0) {
        perror("Failed to create socket");
    }

    return ServerSocket;
}

void HandlingSSL::BindSocket(int ServerSocket) {
    sockaddr_in ServerAddress{};
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_addr.s_addr = INADDR_ANY ;
    ServerAddress.sin_port = htons(port);

    if (bind(ServerSocket, reinterpret_cast<sockaddr *>(&ServerAddress), sizeof(ServerAddress)) < 0) {
        perror("Failed to bind socket");
    }
}

void HandlingSSL::Listen(int ServerSocket) {
    if (listen(ServerSocket, 5)<0) {
        close(ServerSocket);
        perror("Failed to listen on socket");
    }
}

void HandlingSSL::Accept() {
    sockaddr_in ClientAddress{} ;
    socklen_t ClientAddressLength = sizeof(ClientAddress);

    Initialize();
    SSL_CTX* ctx = CreateContext();
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        abort();
    }
    ConfigureContext(ctx);

    int ServerSocket = CreateSocket();
    BindSocket(ServerSocket);
    Listen(ServerSocket);

    while (true) {
        int ClientSocket = accept(ServerSocket, reinterpret_cast<struct sockaddr *>(&ClientAddress), &ClientAddressLength);
        if (ClientSocket < 0) {
            perror("Failed to accept connection");
        }

        SSL* ssl = SSL_new(ctx);
        SSL_set_fd(ssl, ClientSocket);
        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
            SSL_free(ssl);
            close(ClientSocket);
            continue;
        }

        char Buff[BUFFER_SIZE];
        ssize_t bytes_read = SSL_read(ssl, Buff, sizeof(Buff) - 1);
        if (bytes_read > 0) {
            Buff[bytes_read] = '\0';
            HandlingReq_ProvidingResp::GetAndServe(ClientSocket, Buff);
        } else {
            ERR_print_errors_fp(stderr);
            close(ClientSocket);
            continue;
        }

        SSL_shutdown(ssl);
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close (ClientSocket);
     }

    SSL_CTX_free(ctx);
    EVP_cleanup();
}
