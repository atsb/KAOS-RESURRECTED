#include "std.hpp"
#include "mm4.hpp"
#include "fastmem.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <limits>
#ifdef _WIN32
#include <io.h>
#else
#include <sys/types.h>
#endif

struct MMBlock {
    byte *data = nullptr;
    word size = 0;
    byte flags = BLF_UNUSED;
    byte lock = 0;
};

static std::vector<MMBlock> blocks;
static std::vector<std::uint32_t> disk_offsets;
static std::vector<word> disk_sizes;
static std::FILE *datafile = nullptr;
static unsigned long alloc_volume = 0;
static unsigned long alloc_free = 0;



static int mm_maxmemblocks = MAXMEMBLOCKS;
static int mm_memblocks = 0;

static const char idBDF[9] = "NICbdf\x7\x1a";

static bool mm_seek(std::FILE *f, std::uint64_t offset)
{
#ifdef _WIN32
    return _fseeki64(f, static_cast<__int64>(offset), SEEK_SET) == 0;
#else
    return fseeko(f, static_cast<off_t>(offset), SEEK_SET) == 0;
#endif
}

static std::uint16_t read_le16(std::FILE *f)
{
    byte b[2];
    if (std::fread(b, 1, 2, f) != 2)
        return 0;
    return static_cast<std::uint16_t>(b[0] | (b[1] << 8));
}

static void decrypt_bytes(byte *p, word size)
{
    word count = static_cast<word>(size >> 1);
    while (count) {
        
        
        p[0] = static_cast<byte>(p[0] - static_cast<byte>(count));
        p[1] = static_cast<byte>(p[1] - static_cast<byte>(count));
        p += 2;
        --count;
    }
}

static void uncompress(const byte *src, std::size_t compressed_size,
                       byte *dst, std::size_t expected_size)
{
    std::size_t si = 0, di = 0;

    while (si < compressed_size && di < expected_size) {
        const std::int8_t count = static_cast<std::int8_t>(src[si++]);

        if (count >= 0) {
            if (si >= compressed_size)
                break;

            const std::size_t run = static_cast<std::size_t>(count) + 1;
            const byte value = src[si++];

            const std::size_t n = min(run, expected_size - di);
            std::memset(dst + di, value, n);
            di += n;
        } else {
            const std::size_t literal =
                static_cast<std::size_t>(-count) + 1;

            const std::size_t available =
                min(literal, compressed_size - si);
            const std::size_t n =
                min(available, expected_size - di);

            std::memcpy(dst + di, src + si, n);
            di += n;
            si += available;
        }
    }

    if (di < expected_size)
        std::memset(dst + di, 0, expected_size - di);
}

static int new_block()
{
    

    if (mm_memblocks >= mm_maxmemblocks)
        return -1;

    if (mm_memblocks >= static_cast<int>(blocks.size()))
        blocks.resize(static_cast<std::size_t>(mm_memblocks) + 1);

    blocks[mm_memblocks] = MMBlock{};
    return mm_memblocks;
}

char *mm_reserve(word size)
{
    byte *p = new (std::nothrow) byte[size];
    if (!p)
        error("mm_reserve", "out of memory");
    return reinterpret_cast<char *>(p);
}

int mm_alloc(word size)
{
    const int id = new_block();
    if (id < 0)
        error("mm_alloc", "no rooms");

    if (size == 0) {
        if (id >= static_cast<int>(disk_sizes.size()))
            error("mm_alloc", "invalid data block");
        size = disk_sizes[id];
    } else {
        if (size == 0xffffu)
            error("mm_alloc", "block size exceeds BDF format limit");
        size = static_cast<word>((static_cast<unsigned>(size) + 1u) & 0xfffeu);
    }

    blocks[id].size = size;
    blocks[id].flags = datafile ? BLF_DISK : 0;
    blocks[id].lock = 0;

    alloc_volume += size;
    ++mm_memblocks;
    return id;
}

word mm_getblocksize(int id)
{
    if (id < 0 || id >= static_cast<int>(blocks.size()))
        return 0;
    return blocks[id].size;
}

void *mm_recall(int id)
{
    if (id < 0 || id >= static_cast<int>(blocks.size()))
        return nullptr;

    MMBlock &block = blocks[id];
    if (block.flags == BLF_UNUSED)
        return nullptr;

    if (block.data) {
        block.flags |= BLF_USED;
        return block.data;
    }

    block.data = new (std::nothrow) byte[block.size];
    if (!block.data)
        return nullptr;

    if (datafile && id < static_cast<int>(disk_offsets.size())) {
        const std::uint32_t start = disk_offsets[id];
        const std::uint32_t end = disk_offsets[id + 1];
        const std::size_t compressed_size = end - start;

        std::vector<byte> compressed(compressed_size);

        if (!mm_seek(datafile, start)) {
            delete[] block.data;
            block.data = nullptr;
            return nullptr;
        }
        if (compressed_size &&
            std::fread(compressed.data(), 1, compressed_size, datafile)
                != compressed_size) {
            delete[] block.data;
            block.data = nullptr;
            return nullptr;
        }

        decrypt_bytes(compressed.data(),
                      static_cast<word>(min<std::size_t>(
                          compressed_size, 0xffffu)));

        uncompress(compressed.data(), compressed_size,
                   block.data, block.size);
    } else {
        std::memset(block.data, 0, block.size);
    }

    block.flags |= BLF_USED;
    alloc_free = (alloc_free >= block.size)
        ? alloc_free - block.size : 0;
    return block.data;
}

void mm_unload(int id)
{
    if (id < 0 || id >= static_cast<int>(blocks.size()))
        return;

    MMBlock &block = blocks[id];
    if (block.lock)
        return;

    delete[] block.data;
    block.data = nullptr;

    if (block.flags & BLF_DISK)
        block.flags = BLF_DISK;
    else
        block.flags = 0;
}

byte mm_lock(int id)
{
    if (id < 0 || id >= static_cast<int>(blocks.size()))
        return 0;
    return ++blocks[id].lock;
}

byte mm_unlock(int id)
{
    if (id < 0 || id >= static_cast<int>(blocks.size()))
        return 0;

    if (blocks[id].lock)
        --blocks[id].lock;

    return blocks[id].lock;
}

unsigned long mm_memavail()
{
    return ~0UL;
}

unsigned long mm_totalloc()
{
    return alloc_volume;
}

unsigned long mm_freealloc()
{
    return alloc_free;
}

void mm_reset()
{
    for (MMBlock &block : blocks) {
        if (block.flags != BLF_UNUSED && !block.lock)
            block.flags &= static_cast<byte>(~BLF_USED);
    }
}

void mm_resetall()
{
    mm_reset();
}

void mm_flushall()
{
    for (int i = 0; i < static_cast<int>(blocks.size()); ++i)
        mm_unload(i);
}

void mm_init()
{
    mm_done();

    blocks.clear();
    disk_offsets.clear();
    disk_sizes.clear();
    alloc_volume = 0;
    alloc_free = 0;
    mm_memblocks = 0;
    mm_maxmemblocks = MAXMEMBLOCKS;

#ifndef __MAKEDATA__
    datafile = std::fopen(DATAFILENAME, "rb");
#else
    datafile = nullptr;
#endif
    if (datafile) {
        char id[8];
        if (std::fread(id, 1, 8, datafile) != 8 ||
            std::memcmp(id, idBDF, 8) != 0) {
            std::fclose(datafile);
            datafile = nullptr;
            error("mm_init", "invalid KAOS.BDF");
            return;
        }

        const word maxblocks = read_le16(datafile);
        mm_maxmemblocks = static_cast<int>(maxblocks);
        disk_sizes.resize(maxblocks);
        disk_offsets.resize(static_cast<std::size_t>(maxblocks) + 1);

        for (word i = 0; i < maxblocks; ++i)
            disk_sizes[i] = read_le16(datafile);

        std::uint32_t offset =
            10u + static_cast<std::uint32_t>(maxblocks) * 4u;

        for (word i = 0; i < maxblocks; ++i) {
            const word compressed = read_le16(datafile);
            disk_offsets[i] = offset;
            offset += compressed;
        }
        disk_offsets[maxblocks] = offset;

        

        blocks.reserve(static_cast<std::size_t>(mm_maxmemblocks));
    } else {
        blocks.reserve(MAXMEMBLOCKS);
    }
}

void mm_done()
{
    for (MMBlock &block : blocks) {
        delete[] block.data;
        block.data = nullptr;
    }

    blocks.clear();
    disk_offsets.clear();
    disk_sizes.clear();

    if (datafile) {
        std::fclose(datafile);
        datafile = nullptr;
    }

    alloc_volume = 0;
    alloc_free = 0;
    mm_memblocks = 0;
}

#ifdef __VISUALIZE__
void mm_visualize() {}
#endif

#ifdef __MAKEDATA__
static bool mm_write_le16(std::FILE *f, std::uint16_t value)
{
    const byte b[2] = { static_cast<byte>(value),
                        static_cast<byte>(value >> 8) };
    return std::fwrite(b, 1, sizeof(b), f) == sizeof(b);
}

static std::vector<byte> mm_compress(const byte *src, std::size_t size)
{
    std::vector<byte> out;
    out.reserve(size + size / 16 + 16);
    std::size_t pos = 0;
    while (pos < size) {
        std::size_t run = 1;
        while (pos + run < size && run < 128 && src[pos + run] == src[pos])
            ++run;
        if (run >= 3) {
            out.push_back(static_cast<byte>(run - 1));
            out.push_back(src[pos]);
            pos += run;
            continue;
        }

        const std::size_t start = pos;
        std::size_t literal = 0;
        while (pos < size && literal < 128) {
            run = 1;
            while (pos + run < size && run < 128 && src[pos + run] == src[pos])
                ++run;
            if (run >= 3 && literal != 0)
                break;
            ++pos;
            ++literal;
        }
        if (literal == 1) {
            


            out.push_back(0);
            out.push_back(src[start]);
        } else {
            out.push_back(static_cast<byte>(256u - (literal - 1u)));
            out.insert(out.end(), src + start, src + start + literal);
        }
    }
    return out;
}

static void mm_encrypt(std::vector<byte> &data)
{
    const std::size_t pairs = data.size() >> 1;
    for (std::size_t i = 0; i < pairs; ++i) {
        const byte delta = static_cast<byte>(pairs - i);
        data[i * 2] = static_cast<byte>(data[i * 2] + delta);
        data[i * 2 + 1] = static_cast<byte>(data[i * 2 + 1] + delta);
    }
}

void mm_makedatafile()
{
    const std::size_t count = blocks.size();
    if (count == 0)
        error("mm_makedatafile", "no data blocks");
    if (count > MAXMEMBLOCKS || count > 0xffffu)
        error("mm_makedatafile", "too many blocks");

    struct PackedBlock {
        std::vector<byte> data;
        std::uint16_t raw_size = 0;
    };
    std::vector<PackedBlock> packed(count);

    for (std::size_t i = 0; i < count; ++i) {
        MMBlock &block = blocks[i];
        if (block.size == 0)
            error("mm_makedatafile", "zero-sized data block");
        if (!block.data) {
            block.data = new (std::nothrow) byte[block.size];
            if (!block.data)
                error("mm_makedatafile", "out of memory");
            std::memset(block.data, 0, block.size);
        }
        packed[i].raw_size = block.size;
        packed[i].data = mm_compress(block.data, block.size);
        if (packed[i].data.size() > 0xffffu)
            error("mm_makedatafile", "compressed block too large");
        mm_encrypt(packed[i].data);
    }

    std::FILE *f = std::fopen(DATAFILENAME, "wb");
    if (!f)
        error("mm_makedatafile", "cannot create KAOS.BDF");

    bool ok = std::fwrite(idBDF, 1, 8, f) == 8 &&
              mm_write_le16(f, static_cast<std::uint16_t>(count));
    for (const PackedBlock &b : packed)
        ok = ok && mm_write_le16(f, b.raw_size);
    for (const PackedBlock &b : packed)
        ok = ok && mm_write_le16(f, static_cast<std::uint16_t>(b.data.size()));
    for (const PackedBlock &b : packed) {
        if (!b.data.empty())
            ok = ok && std::fwrite(b.data.data(), 1, b.data.size(), f) == b.data.size();
    }
    if (std::fclose(f) != 0)
        ok = false;
    if (!ok)
        error("mm_makedatafile", "write failed");
}
#endif
