#pragma once
#ifndef KAOS_PLATFORM_COMPAT_HPP
#define KAOS_PLATFORM_COMPAT_HPP

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <cstdint>
#include <fcntl.h>
#include <sys/types.h>

#if defined(_WIN32)
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <io.h>
#  include <windows.h>
#  include <sys/stat.h>
#else
#  include <unistd.h>
#  include <sys/stat.h>
#endif

#ifndef O_BINARY
#  define O_BINARY 0
#endif

#if defined(_WIN32)
#  ifndef lseek
#    define lseek _lseek
#  endif
#endif

#if defined(_WIN32)
 





static inline int kaos_creat(const char *path, int  )
{
    return ::_creat(path, _S_IREAD | _S_IWRITE);
}
#  ifndef KAOS_NO_DOS_CREAT_ALIAS
#    define _creat(path, mode) kaos_creat((path), (mode))
#  endif
#endif


 






template <typename T, typename U>
constexpr typename std::common_type<T, U>::type min(T a, U b)
{
    using R = typename std::common_type<T, U>::type;
    const R ra = static_cast<R>(a);
    const R rb = static_cast<R>(b);
    return ra < rb ? ra : rb;
}

template <typename T, typename U>
constexpr typename std::common_type<T, U>::type max(T a, U b)
{
    using R = typename std::common_type<T, U>::type;
    const R ra = static_cast<R>(a);
    const R rb = static_cast<R>(b);
    return ra > rb ? ra : rb;
}

 








int kaos_rand();
int kaos_random(int maximum);
unsigned kaos_random_seed();
void kaos_srand(unsigned seed);
void kaos_randomize();
#ifndef random
#  define random(maximum) kaos_random(maximum)
#endif

int kaos_kbhit();
int kaos_getch();
#ifndef kbhit
#  define kbhit() kaos_kbhit()
#endif
#ifndef getch
#  define getch() kaos_getch()
#endif
#ifndef randomize
#  define randomize() kaos_randomize()
#endif

#ifndef far
#  define far
#endif
#ifndef near
#  define near
#endif
#ifndef interrupt
#  define interrupt
#endif
#ifndef _saveregs
#  define _saveregs
#endif
#ifndef _loadds
#  define _loadds
#endif

#if !defined(_WIN32)
static inline int _open(const char *path, int flags, ...) { return ::open(path, flags, 0666); }
static inline int _close(int fd) { return ::close(fd); }
static inline long _lseek(int fd, long offset, int origin) { return (long)::lseek(fd, offset, origin); }
static inline int _read(int fd, void *buf, unsigned count) { return (int)::read(fd, buf, count); }
static inline int _write(int fd, const void *buf, unsigned count) { return (int)::write(fd, buf, count); }
#endif

#if !defined(_WIN32)
static inline int _creat(const char *path, int mode) { (void)mode; return ::open(path, O_CREAT | O_TRUNC | O_WRONLY, 0666); }
static inline long _tell(int fd) { return (long)::lseek(fd, 0, SEEK_CUR); }
static inline long _filelength(int fd) {
    const long pos = (long)::lseek(fd, 0, SEEK_CUR);
    const long end = (long)::lseek(fd, 0, SEEK_END);
    ::lseek(fd, pos, SEEK_SET);
    return end;
}
#endif

#ifndef tell
#  define tell(fd) _tell(fd)
#endif
#ifndef filelength
#  define filelength(fd) _filelength(fd)
#endif

static inline int kaos_eof(int fd)
{
    const auto pos = _lseek(fd, 0, SEEK_CUR);
    if (pos < 0) return 1;
    const auto end = _lseek(fd, 0, SEEK_END);
    if (end < 0) return 1;
    _lseek(fd, pos, SEEK_SET);
    return pos >= end;
}

#ifndef KAOS_NO_DOS_EOF_ALIAS
#  define eof(fd) kaos_eof(fd)
#endif

static inline unsigned char inportb(unsigned short) { return 0; }
static inline void outportb(unsigned short, unsigned char) {}
static inline void disable() {}
static inline void enable() {}

using kaos_interrupt_handler = void (*)(...);
static inline kaos_interrupt_handler getvect(int) { return nullptr; }
static inline void setvect(int, kaos_interrupt_handler) {}

static inline void sound(unsigned, unsigned) {}
static inline void nosound() {}

static inline unsigned long coreleft() { return 640UL * 1024UL; }

#endif
