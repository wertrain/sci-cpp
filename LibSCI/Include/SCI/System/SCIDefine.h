#pragma once

/// <summary>
/// ���O��Ԓ�`
/// </summary>
#define NS_DEF_SCI sci
#define NS_DEF_SCI_SYS sys

#define NS_SCI NS_DEF_SCI
#define NS_SCI_SYS NS_DEF_SCI::NS_DEF_SCI_SYS

#define NS_SCI_BEGIN namespace NS_DEF_SCI {
#define NS_SCI_END }
#define NS_SCI_SYS_BEGIN NS_SCI_BEGIN namespace NS_DEF_SCI_SYS {
#define NS_SCI_SYS_END } NS_SCI_END
