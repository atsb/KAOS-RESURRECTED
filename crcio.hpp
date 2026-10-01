#pragma once
 






extern int CRC_openread(const char *filename);
extern int CRC_openwrite(const char *filename);
extern int CRC_openread_user(const char *filename);
extern int CRC_openwrite_user(const char *filename);
extern int CRC_eof(int channel);
extern long long CRC_remaining(int channel);
extern void CRC_read(int channel, void *ptr, word size);
extern void CRC_write(int channel, const void *ptr, word size);
extern void CRC_lseek(int channel, long displ, int from);
extern int CRC_close(int channel);
