#include "Chronometer.hpp"
#include <string>

void Chronometer::Restart() {
    timer = svcGetSystemTick();
}

u64 Chronometer::GetElapsedTimeInMillisenconds() {
    u64 current = svcGetSystemTick();
	u64 diff = current - timer;
	return diff / CPU_TICKS_PER_MSEC;
}

Chronometer::Chronometer() {
    Restart();
}

Chronometer::~Chronometer() {
    delete &timer;
}

u64 Chronometer::GetElapsedSeconds() {
    return (GetElapsedTimeInMillisenconds() / 1000) % 60;
}

u64 Chronometer::GetElapsedMinutes() {
    return (GetElapsedTimeInMillisenconds() / 1000) / 60;
}

void Chronometer::GetElapsedTimeInStringFormat(char* res) {
    int min = (int) GetElapsedMinutes();
    int sec = (int) GetElapsedSeconds();
    sprintf(res, "%02d:%02d\n", min, sec);
}
