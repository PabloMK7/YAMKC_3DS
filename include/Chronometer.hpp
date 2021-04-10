#pragma once
#include <3ds.h>

#define SYSCLOCK_SOC       (16756991)
#define SYSCLOCK_ARM9      (SYSCLOCK_SOC * 8)
#define SYSCLOCK_ARM11     (SYSCLOCK_ARM9 * 2)
#define SYSCLOCK_ARM11_NEW (SYSCLOCK_ARM11 * 3)

#define CPU_TICKS_PER_MSEC (SYSCLOCK_ARM11 / 1000.0)
#define CPU_TICKS_PER_USEC (SYSCLOCK_ARM11 / 1000000.0)

class Chronometer
{
private:
    u64 timer;
public:
    Chronometer();
    ~Chronometer();
    void Restart();
    u64 GetElapsedTimeInMillisenconds();
    u64 GetElapsedMinutes();
    u64 GetElapsedSeconds();
    void GetElapsedTimeInStringFormat(char* res);
};
