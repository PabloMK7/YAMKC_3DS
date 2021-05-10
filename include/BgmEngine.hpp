#include "Sound.hpp"

class BgmEngine
{
private:
    Sound* startBgm;
    Sound* mainBgm;
    Sound* finishBgm;
    Sound* finalLapBgm;
    int fastVersionFrames = 0;
public:
    enum class BgmType {
        START,
        MAIN,
        FASTMAIN,
        FINISH,
    };
    BgmEngine();
    ~BgmEngine();
    void PlayBGM(BgmType bgm);
    void StopBGM();
    void Tick();
};
