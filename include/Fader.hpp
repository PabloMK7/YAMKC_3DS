#include "Plane.hpp"
#include "Vector.hpp"
#include "Material.hpp"

class Fader : public Plane
{
private:

public:
    Fader();
    ~Fader();
    void SetColor(const Color& color);
    void SetFade(float amount);
};
