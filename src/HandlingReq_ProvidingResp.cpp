//
// Created by ignorant05 on 1/23/25.
//

#include "HandlingReq_ProvidingResp.h"

#include <functional>
#include <sstream>
#include <string>
#include <unistd.h>
#include <unordered_set>
#include <sys/socket.h>

using RequestHandler = std::function<void(std::string& header, std::string& body)> ;

void HandlingGET(std::string& header, std::string& body);
void HandlingPOST(std::string& header, std::string& body);
void HandlingPUT(std::string& header, std::string& body);
void HandlingDELETE(std::string& header, std::string& body);
void HandlingPATCH(std::string& header, std::string& body);
void HandlingOPTIONS(std::string& header, std::string& body);

void HandlingGET (std::string& Header, std::string& Body){
    Header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
    Body = "Hello, this is a GET response!";
}

void HandlingPOST (std::string& Header, std::string& Body) {
    Header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
    Body = "Hello, this is a POST response!";
}

void HandlingPUT(std::string& Header, std::string& Body) {
    Header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
    Body = "Hello, this is a PUT response!";
}

void HandlingDELETE(std::string& Header, std::string& Body){
    Header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
    Body = "Hello, this is a DELETE response!";
}

void HandlingPATCH(std::string& Header, std::string& Body){
    Header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
    Body = "Hello, this is a PATCH response!";
}

void HandlingOPTIONS(std::string& Header, std::string& Body) {
    Header = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
    Body = "Hello, this is a OPTIONS response!";
}

void PermissionDeniedHandling(std::string& Header, std::string& Body) {
    Header = "HTTP/1.1 301 Permission Denied! \r\nContent-Type: text/plain\r\n\r\n";
    Body= "Permission Denied";
}
void NotFoundErrorHandling(std::string& Header, std::string& Body){
    Header = "HTTP/1.1 404 Invalid Method/Path! \r\nContent-Type: text/plain\r\n\r\n";
    Body= "Invalid Method/Path";
}

enum class Methods {
    GET,
    POST,
    PUT,
    PATCH,
    DELETE,
    OPTIONS,
    NOT_DEFINED
};

RequestHandler GetHandler[7] ={
    HandlingGET,
    HandlingPOST,
    HandlingPUT,
    HandlingPATCH,
    HandlingDELETE,
    HandlingOPTIONS,
    NotFoundErrorHandling
};

Methods GetMethod(const std::string &Method) {
    if (Method == "GET") return Methods::GET;
    else if (Method == "POST") return Methods::POST;
    else if (Method == "PUT") return Methods::PUT ;
    else if (Method == "PATCH") return Methods::PATCH;
    else if (Method == "DELETE") return Methods::DELETE;
    else if (Method == "OPTIONS") return Methods::OPTIONS;
    else return Methods::NOT_DEFINED;
}

void HandlingReq_ProvidingResp::GetAndServe(int ClientSocket, const char* Request) {
    std::string Body, Header;
    char Method[10], Path[100];
    std::istringstream iss(Request);
    iss >> Method >> Path;

    std::unordered_set<std::string> PermittedToNonEmployeePathsList = {
        "/", "/login", "/index.html", "/index.php", "/main.php", "/index.js", "/index.json"
        };

    std::unordered_set<std::string> NotPermittedToNonEmployeePathsList = {
        "/admin", "/phpmyadmin", "/resources", "/payment.php", "main.cpp", "/code"
        };

    Methods M = GetMethod(Method);

    std::unordered_set<std::string>::iterator FoundInPermittedToNonEmployeePaths = PermittedToNonEmployeePathsList.find(Path);

    std::unordered_set<std::string>::iterator FoundInNotPermittedToNonEmployeePaths = NotPermittedToNonEmployeePathsList.find(Path);

    if (
        (FoundInPermittedToNonEmployeePaths == PermittedToNonEmployeePathsList.end()) &&
        (FoundInNotPermittedToNonEmployeePaths != NotPermittedToNonEmployeePathsList.end()) &&
        (M != Methods::NOT_DEFINED)
        )
        PermissionDeniedHandling(Header,Body);
    else if (
        (FoundInPermittedToNonEmployeePaths != PermittedToNonEmployeePathsList.end() )&&
        (FoundInNotPermittedToNonEmployeePaths == NotPermittedToNonEmployeePathsList.end()) &&
        (M != Methods::NOT_DEFINED)
        )
        GetHandler[static_cast<int>(M)](Header, Body);

    else NotFoundErrorHandling(Header,Body);

    send(ClientSocket, Header.c_str(), Header.length(), 0);
    send(ClientSocket, Body.c_str(), Body.length(), 0);

    close (ClientSocket);
}
