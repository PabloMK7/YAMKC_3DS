#include "BgmEngine.hpp"
#include "Kart.hpp"
#include "Chronometer.hpp"
#include "CountdownDisplay.hpp"
#include "LapCounter.hpp"

class RaceEngine
{
private:
    enum class SequenceState {
        NONE,
        PRECOUNTDOWN,
        COUNTDOWN,
        PRERACE,
        RACE,
        FINISH,
        END
    };
    SequenceState currState;
    SequenceState prevState;
    BgmEngine* bgmEngine;
    Sound* count321;
    Sound* countGo;
    Sound* secondLap;
    Sound* finishRace;
    Kart* kart;
    Fader* raceFader;
    Chronometer* chronometer;
    CountdownDisplay* countdownDisp;
    LapCounter* lapCounterDisp;
    int countdownTimer;
    int prevLap;
public:
    RaceEngine(Kart* playerKart, Chronometer* chrono, CountdownDisplay* cntdwn, LapCounter* lapcnt);
    ~RaceEngine();
    void OnLap(int lap);
    void Calc();
    void Draw();
};
