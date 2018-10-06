#include <SCI/Server/SCIServer.h>

namespace sci
{

class SCIServer::Impl
{

};

SCIServer::SCIServer()
    : mImpl(new SCIServer::Impl)
{

}

}; // namespace sci
