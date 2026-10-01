 






 
 

#include "std.hpp"
#include "mm4.hpp"
#include "fastmem.hpp"
#include "crcio.hpp"
#if defined(KAOS_EDITOR) && defined(_WIN32)
#include "win32_editor_platform.hpp"
#else
#include "kaos_sdl.hpp"
#include <SDL3/SDL.h>
#endif
#include <cctype>
#include <cstring>
#include <string>
#ifdef _WIN32
#include <sys/stat.h>
#endif
#ifndef _WIN32
#include <dirent.h>
#endif

#define	CRC_MAXCHANNELS		4
#define	CRC_BUFLEN			8192

#define	CRC_validchannel(x)	(x>=0 && x<CRC_MAXCHANNELS)

#define	CRCS_IDLE		0
#define	CRCS_READING	1
#define	CRCS_WRITING 	2

static int CRC_channels = 0;
static int CRC_handle[CRC_MAXCHANNELS] =
		{-1,-1,-1,-1};
static word CRC_tot[CRC_MAXCHANNELS] =
		{0,0,0,0};
static char CRC_status[CRC_MAXCHANNELS] =
		{CRCS_IDLE,CRCS_IDLE,CRCS_IDLE,CRCS_IDLE};
static bool CRC_failed[CRC_MAXCHANNELS] = {false,false,false,false};

void CRC_reset(int channel) {
	CRC_tot[channel]=0;
}

void CRC_add(int channel, const void *p, word size) {
    if (!CRC_validchannel(channel) || !p) return;
    const byte *data = static_cast<const byte *>(p);
    word total = CRC_tot[channel];
    for (word i = 0; i < size; ++i)
        total = static_cast<word>(total + data[i]);
    CRC_tot[channel] = total;
}

char CRC_check(int channel) {
	int handle = CRC_handle[channel];
	if (handle<0) return 0;
	long initpos, size;
	void *p = mm_reserve(CRC_BUFLEN);
	word toread;
	CRC_reset(channel);
	const long file_size = filelength(handle);
	initpos = tell(handle);
	if (file_size < 0 || initpos < 0 || file_size < initpos + 2l) {
		delete[] static_cast<byte *>(p);
		return 0;
	}
	size = file_size - initpos - 2l; 
	while (size > 0) {
		toread = static_cast<word>(size > (long)CRC_BUFLEN ? CRC_BUFLEN : size);
		const int n = _read(handle,p,toread);
		if (n != toread) {
			delete[] static_cast<byte *>(p);
			return 0;
		}
		CRC_add(channel,p,toread);
		size -= toread;
	}
	delete[] static_cast<byte *>(p);
	if (_read(handle,&toread,sizeof(toread)) != sizeof(toread))
		return 0;
	lseek(handle,initpos,SEEK_SET);
	return toread==CRC_tot[channel];
}

#pragma warn -rvl
int CRC_newchannel() {
	for (int i=0; i<CRC_MAXCHANNELS; i++)
		if (CRC_status[i]==CRCS_IDLE) {
			CRC_channels++;
			return i;
		}
	error("CRC_newchannel","fail");
	return -1;
}
#pragma warn +rvl

#ifndef _WIN32


static int crc_open_ci(const char *filename)
{
	int handle = _open(filename, O_RDONLY | O_BINARY);
	if (handle >= 0)
		return handle;

	const char *slash = std::strrchr(filename, '/');
	std::string dir = ".";
	std::string leaf = filename;
	if (slash) {
		dir.assign(filename, static_cast<std::size_t>(slash - filename));
		leaf = slash + 1;
		if (dir.empty())
			dir = "/";
	}

	DIR *d = opendir(dir.c_str());
	if (!d)
		return -1;

	auto eq_ci = [](const char *a, const char *b) {
		while (*a && *b) {
			if (std::tolower(static_cast<unsigned char>(*a)) !=
				std::tolower(static_cast<unsigned char>(*b)))
				return false;
			++a; ++b;
		}
		return *a == *b;
	};

	std::string resolved;
	while (dirent *ent = readdir(d)) {
		if (eq_ci(ent->d_name, leaf.c_str())) {
			if (dir == ".")
				resolved = ent->d_name;
			else if (dir == "/")
				resolved = std::string("/") + ent->d_name;
			else
				resolved = dir + "/" + ent->d_name;
			break;
		}
	}
	closedir(d);
	if (resolved.empty())
		return -1;
	return _open(resolved.c_str(), O_RDONLY | O_BINARY);
}
#endif

int CRC_openread(const char *filename) {
	if (CRC_channels<CRC_MAXCHANNELS) {
#ifdef _WIN32
		int handle = _open(filename,O_RDONLY|O_BINARY);
#else
		int handle = crc_open_ci(filename);
#endif
		if (handle < 0) {
#if defined(KAOS_EDITOR) && defined(_WIN32)
			const char *base = kaos_editor_win32::executable_directory();
			if (base) {
				std::string path(base);
				if (!path.empty() && path.back() != '\\' && path.back() != '/') path += '\\';
				path += filename;
				handle = _open(path.c_str(), O_RDONLY|O_BINARY);
			}
#else
			const char *base = SDL_GetBasePath();
			if (base) {
				std::string path(base);
				path += filename;
#ifdef _WIN32
				handle = _open(path.c_str(), O_RDONLY|O_BINARY);
#else
				handle = crc_open_ci(path.c_str());
#endif
			}
#endif
		}
		if (handle>=0) {
			int channel = CRC_newchannel();
			CRC_handle[channel] = handle;
			CRC_status[channel] = CRCS_READING;
			CRC_failed[channel] = false;
			if (CRC_check(channel))
				return channel;
			CRC_close(channel);
		}
	}
	return -1;
}

int CRC_openwrite(const char *filename) {
	if (CRC_channels<CRC_MAXCHANNELS) {
		


#ifdef _WIN32
		int handle = _open(filename, _O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY,
			_S_IREAD | _S_IWRITE);
#else
		int handle = _creat(filename, 0666);
#endif
		if (handle>=0) {
			int channel = CRC_newchannel();
			CRC_handle[channel] = handle;
			CRC_reset(channel);
			CRC_status[channel] = CRCS_WRITING;
			CRC_failed[channel] = false;
			return channel;
		}
	}
	return -1;
}




int CRC_openread_user(const char *filename) {
#if defined(KAOS_EDITOR) && defined(_WIN32)
	return CRC_openread(filename);
#else
	char *path = kaos_sdl_get_user_path(filename);
	if (path) {
		const int handle = CRC_openread(path);
		SDL_free(path);
		if (handle >= 0) return handle;
	}
	return CRC_openread(filename); 
#endif
}

int CRC_openwrite_user(const char *filename) {
#if defined(KAOS_EDITOR) && defined(_WIN32)
	return CRC_openwrite(filename);
#else
	char *path = kaos_sdl_get_user_path(filename);
	if (!path) return CRC_openwrite(filename);
	const int handle = CRC_openwrite(path);
	SDL_free(path);
	return handle;
#endif
}

int CRC_eof(int handle) {
	return eof(handle);
}

long long CRC_remaining(int channel) {
	if (!CRC_validchannel(channel) || CRC_handle[channel] < 0) return 0;
#ifdef _WIN32
	const __int64 pos = _telli64(CRC_handle[channel]);
	const __int64 end = _filelengthi64(CRC_handle[channel]);
	if (pos < 0 || end < pos) return 0;
	return static_cast<long long>(end - pos);
#else
	const off_t pos = lseek(CRC_handle[channel], 0, SEEK_CUR);
	const off_t end = lseek(CRC_handle[channel], 0, SEEK_END);
	if (pos < 0 || end < pos) return 0;
	lseek(CRC_handle[channel], pos, SEEK_SET);
	return static_cast<long long>(end - pos);
#endif
}

void CRC_read(int channel, void *ptr, word size) {
	 
	if (size) _read(CRC_handle[channel],ptr,size);
	 
}

void CRC_write(int channel, const void *ptr, word size) {
	if (!CRC_validchannel(channel) || CRC_status[channel]!=CRCS_WRITING || !ptr) return;
	const byte *data = static_cast<const byte *>(ptr);
	word written = 0;
	while (written < size) {
		const int n = _write(CRC_handle[channel], data + written, size - written);
		if (n <= 0) { CRC_failed[channel] = true; return; }
		written = static_cast<word>(written + n);
	}
	CRC_add(channel,ptr,size);
	 
}

void CRC_lseek(int channel, long displ, int from) {
	 
	lseek(CRC_handle[channel],displ,from);
	 
}

int CRC_close(int channel) {
	if (!CRC_validchannel(channel) ||
		CRC_status[channel]==CRCS_IDLE) return 0;
	int ok = !CRC_failed[channel];
	if (CRC_status[channel]==CRCS_WRITING && ok) {
		const byte *data = reinterpret_cast<const byte *>(&CRC_tot[channel]);
		std::size_t left = sizeof(CRC_tot[channel]);
		while (left) {
			const int n = _write(CRC_handle[channel], data, static_cast<unsigned>(left));
			if (n <= 0) { ok = 0; break; }
			data += n;
			left -= static_cast<std::size_t>(n);
		}
	}
	if (_close(CRC_handle[channel]) != 0) ok = 0;
	CRC_status[channel] = CRCS_IDLE;
	CRC_failed[channel] = false;
	CRC_channels--;
	return ok;
}
