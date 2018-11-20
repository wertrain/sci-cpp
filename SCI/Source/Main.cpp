#include <iostream>
#include <SCI/SCI.h>

int main()
{
    sci::SCIInitialize();

    const int port = 101012;
    const char* address = "127.0.0.1:8080";

#ifndef _DEBUG
    sci::SCIClient client;
    client.Connect(port, address);
#else
    sci::SCIServer server;
    server.Start(port, address);
#endif

    sci::SCIFinalize();

    return 0;
}
