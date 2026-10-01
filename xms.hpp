#pragma once
#ifndef KAOS_XMS_HPP
#define KAOS_XMS_HPP
#include "std.hpp"
struct TXMSMoveStruct {
    unsigned long size;
    word shandle;
    void *sptr;
    word dhandle;
    void *dptr;
};
extern char XMSok;
extern char XMSinstalled();
extern int XMSgetver();
extern int XMSgetrev();
extern word XMSmemfree();
extern word XMSblockfree();
extern word XMSnew(word size);
extern char XMSdelete(word handle);
extern char XMSmove(TXMSMoveStruct far *movestruct);
extern char XMSgetsize(word handle, word &size);
#endif
