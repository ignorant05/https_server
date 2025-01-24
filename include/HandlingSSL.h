//
// Created by ignorant05 on 1/22/25.
//
#ifndef HANDLING_SSL_H
#define HANDLING_SSL_H

#include <openssl/err.h>

#include "HandlingReq_ProvidingResp.h"

class HandlingSSL : public HandlingReq_ProvidingResp {
    public:
        static int CreateSocket();
        static void BindSocket(int ServerSocket);
        static void Listen(int ServerSocket);
        static void Accept();
        static void Initialize();
        static SSL_CTX * CreateContext();
        static void ConfigureContext(SSL_CTX * context);
};
#endif //HANDLING_SSL_H
