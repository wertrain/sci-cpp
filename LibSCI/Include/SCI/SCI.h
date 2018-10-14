/**
 * @file SCI.h
 * @brief ���C�u��������
 */
#pragma once

#include <SCI/Client/SCIClient.h>
#include <SCI/Server/SCIServer.h>

namespace sci
{
    /// ���C�u������������
    bool SCIInitialize();
    /// ���C�u�������I��
    void SCIFinalize();
}
