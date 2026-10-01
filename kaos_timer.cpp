#include "kaos_sdl.hpp"
#include <atomic>
#include <thread>
#include <chrono>
#include <cstdint>

extern std::atomic<byte> Sync;
extern char slowmode;
static std::atomic<bool> g_sync_running{false};
static std::atomic<bool> g_sync_enabled{false};
static std::thread g_sync_thread;

namespace {

using steady_clock = std::chrono::steady_clock;

 


constexpr std::uint64_t PIT_CLOCK_HZ = 1193182ull;
constexpr std::uint64_t PIT_DIVISOR = 0x8000ull;
constexpr std::uint64_t BIOS_DIVISOR = 0x10000ull;

struct ExactPeriod {
    std::uint64_t whole_ns;
    std::uint64_t rem;
    std::uint64_t den;
};

constexpr ExactPeriod make_pit_period()
{
    const std::uint64_t n = PIT_DIVISOR * 1000000000ull;
    return { n / PIT_CLOCK_HZ, n % PIT_CLOCK_HZ, PIT_CLOCK_HZ };
}

constexpr ExactPeriod make_bios_period()
{
    const std::uint64_t n = BIOS_DIVISOR * 1000000000ull;
    return { n / PIT_CLOCK_HZ, n % PIT_CLOCK_HZ, PIT_CLOCK_HZ };
}

constexpr ExactPeriod PIT_PERIOD = make_pit_period();
constexpr ExactPeriod BIOS_PERIOD = make_bios_period();

void advance_period(steady_clock::time_point &deadline,
                    std::uint64_t &remainder,
                    const ExactPeriod &period)
{
    deadline += std::chrono::nanoseconds(period.whole_ns);
    remainder += period.rem;
    if (remainder >= period.den) {
        deadline += std::chrono::nanoseconds(1);
        remainder -= period.den;
    }
}

void add_sync_ticks(std::uint64_t ticks)
{
    if (!ticks)
        return;

    byte cur = Sync.load(std::memory_order_relaxed);
    while (ticks && cur < 8) {
        const byte room = static_cast<byte>(8 - cur);
        const byte add = static_cast<byte>(ticks < room ? ticks : room);
        if (Sync.compare_exchange_weak(cur,
                                       static_cast<byte>(cur + add),
                                       std::memory_order_relaxed,
                                       std::memory_order_relaxed))
            return;
    }
}

}  

void kaos_sdl_start_sync()
{
    if (g_sync_running.exchange(true))
        return;

    g_sync_thread = std::thread([] {
        const ExactPeriod &period = slowmode ? BIOS_PERIOD : PIT_PERIOD;
        using clock = steady_clock;

        clock::time_point next = clock::now();
        std::uint64_t remainder = 0;

         

        advance_period(next, remainder, period);

        while (g_sync_running.load(std::memory_order_relaxed)) {
            const auto now = clock::now();
            if (now < next) {
                const auto remaining = next - now;
                if (remaining > std::chrono::milliseconds(2))
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                else
                    std::this_thread::yield();
                continue;
            }

             




            std::uint64_t elapsed = 0;
            const auto sample_now = clock::now();
            do {
                ++elapsed;
                advance_period(next, remainder, period);
            } while (next <= sample_now && elapsed < 256);

             

            if (g_sync_enabled.load(std::memory_order_relaxed))
                add_sync_ticks(elapsed);
        }
    });
}

void kaos_sdl_enable_sync()
{
    g_sync_enabled.store(true, std::memory_order_release);
}

void kaos_sdl_disable_sync()
{
    g_sync_enabled.store(false, std::memory_order_release);
}

void kaos_sdl_stop_sync()
{
    g_sync_enabled.store(false, std::memory_order_release);
    if (!g_sync_running.exchange(false))
        return;

    if (g_sync_thread.joinable())
        g_sync_thread.join();
}
