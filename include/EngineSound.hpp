#include "Sound.hpp"

class EngineSound
{
private:
    enum class StateMachine {
        NONE,
        IDLE,
        START,
        FAST,
        DECEL
    };
    Sound idleSound;
    Sound startSound;
    Sound runSound;
    Sound endSound;
    float speed;
    float prevSpeed;
    float targetPitch = 1.f;
    int soundStartFrames = 30;
    StateMachine state;
    StateMachine prevState;
    void CalcStateMachine();
public:
    
    EngineSound(/* args */);
    ~EngineSound();
    void Calc(float newSpeed);
    void Terminate();
};
