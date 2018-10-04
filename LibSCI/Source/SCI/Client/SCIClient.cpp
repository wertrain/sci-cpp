#include <SCI/Client/SCIClient.h>

namespace sci
{

class SCIClient::Impl
{

};

SCIClient::SCIClient()
    : mImpl(new SCIClient::Impl)
{

}

}; // namespace sci
