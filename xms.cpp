#include "std.hpp"
#include "xms.hpp"

char XMSok = 0;

char XMSinstalled() { return 0; }
int XMSgetver() { return 0; }
int XMSgetrev() { return 0; }
word XMSmemfree() { return 0; }
word XMSblockfree() { return 0; }
word XMSnew(word) { return 0; }
char XMSdelete(word) { return 0; }
char XMSmove(TXMSMoveStruct far *) { return 0; }
char XMSgetsize(word, word &size) { size = 0; return 0; }
