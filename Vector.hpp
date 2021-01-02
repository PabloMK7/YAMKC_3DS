#pragma once
#define PI 3.14159265358979323846f

class Angle
{
public:
    static constexpr Angle Zero() {
        return Angle();
    }

    static constexpr float RadiansToDegrees(float radians) {
        return radians * (180.f / PI);
    }

    static constexpr float DegreesToRadians(float angles) {
        return angles * (PI / 180.f);
    }

    static constexpr Angle FromRadians(float val) {
        return Angle(val);
    }

    static constexpr Angle FromDegrees(float val) {
        return Angle(DegreesToRadians(val));
    }

    inline float AsDegrees() const {
        return RadiansToDegrees(value);
    }

    inline float AsRadians() const {
        return value;
    }

    inline float Cos() const {
        return cosf(value);
    }

    inline float Sin() const {
        return sinf(value);
    }

    void operator=(const Angle& right) {
        this->value = right.value;
    }

    Angle operator+(const Angle& right) const {
        return FromRadians(this->value + right.value);
    }

    void operator+=(const Angle& right) {
        this->value += right.value;
    }

    Angle operator-(const Angle& right) const {
        return FromRadians(this->value - right.value);
    }

    void operator-=(const Angle& right) {
        this->value -= right.value;
    }

    Angle operator*(const float& amount) const {
        return FromRadians(this->value * amount);
    }

    void operator*=(const float& amount) {
        this->value *= amount;
    }

    Angle operator/(const float& amount) const {
        return FromRadians(this->value / amount);
    }

    void operator/=(const float& amount) {
        this->value /= amount;
    }

    void Lerp(const Angle& other, float amount) {
        *this += (other - *this) * amount;
    }

    void Cerp(const Angle& other, float amount) {
        Lerp(other, 1.f - (amount - 1.f) * (amount - 1.f));
    }

private:
    friend class Angle3;
    constexpr Angle() : value(0) {}
    constexpr Angle(float val) : value(val) {}

    // Stored as radians
    float value;
};

class Angle3
{
public:
    constexpr Angle3() : x(), y(), z() {}

    constexpr Angle3(Angle xVal, Angle yVal, Angle zVal) : x(xVal), y(yVal), z(zVal) {}

    void operator=(const Angle3& right)
    {
        this->x = right.x;
        this->y = right.y;
        this->z = right.z;
    }

    Angle3 operator+(const Angle3& right) const
    {
        Angle3 ret;
        ret.x = this->x + right.x;
        ret.y = this->y + right.y;
        ret.z = this->z + right.z;
        return ret;
    }

    void operator+=(const Angle3& right)
    {
        this->x += right.x;
        this->y += right.y;
        this->z += right.z;
    }

    Angle3 operator-(const Angle3& right) const
    {
        Angle3 ret;
        ret.x = this->x - right.x;
        ret.y = this->y - right.y;
        ret.z = this->z - right.z;
        return ret;
    }

    void operator-=(const Angle3& right)
    {
        this->x -= right.x;
        this->y -= right.y;
        this->z -= right.z;
    }

    Angle3 operator*(const float& amount) const
    {
        Angle3 ret;
        ret.x = this->x * amount;
        ret.y = this->y * amount;
        ret.z = this->z * amount;
        return ret;
    }

    void operator*=(const float& amount)
    {
        this->x *= amount;
        this->y *= amount;
        this->z *= amount;
    }

    Angle3 operator/(const float& amount) const
    {
        Angle3 ret;
        ret.x = this->x * amount;
        ret.y = this->y * amount;
        ret.z = this->z * amount;
        return ret;
    }

    void operator/=(const float& amount)
    {
        this->x /= amount;
        this->y /= amount;
        this->z /= amount;
    }

    void Lerp(const Angle3& other, float amount) {
        *this += (other - *this) * amount;
    }

    void Cerp(const Angle3& other, float amount) {
        Lerp(other, 1.f - (amount - 1.f) * (amount - 1.f));
    }

    Angle x;
    Angle y;
    Angle z;
private:

};

class Vector3
{
public:
    constexpr Vector3() : x(0), y(0), z(0) {}

    constexpr Vector3(float xVal, float yVal, float zVal) : x(xVal), y(yVal), z(zVal) {}

    void operator=(const Vector3& right)
    {
        this->x = right.x;
        this->y = right.y;
        this->z = right.z;
    }

    Vector3 operator+(const Vector3& right) const
    {
        Vector3 ret;
        ret.x = this->x + right.x;
        ret.y = this->y + right.y;
        ret.z = this->z + right.z;
        return ret;
    }

    void operator+=(const Vector3& right)
    {
        this->x += right.x;
        this->y += right.y;
        this->z += right.z;
    }

    Vector3 operator-(const Vector3& right) const
    {
        Vector3 ret;
        ret.x = this->x - right.x;
        ret.y = this->y - right.y;
        ret.z = this->z - right.z;
        return ret;
    }

    void operator-=(const Vector3& right)
    {
        this->x -= right.x;
        this->y -= right.y;
        this->z -= right.z;
    }

    Vector3 operator*(const float& amount) const 
    {
        Vector3 ret;
        ret.x = this->x * amount;
        ret.y = this->y * amount;
        ret.z = this->z * amount;
        return ret;
    }

    void operator*=(const float& amount)
    {
        this->x *= amount;
        this->y *= amount;
        this->z *= amount;
    }

    Vector3 operator/(const float& amount) const 
    {
        Vector3 ret;
        ret.x = this->x * amount;
        ret.y = this->y * amount;
        ret.z = this->z * amount;
        return ret;
    }

    void operator/=(const float& amount)
    {
        this->x /= amount;
        this->y /= amount;
        this->z /= amount;
    }

    float Dot(const Vector3& other) const {
        return this->x * other.x + this->y * other.y + this->z * other.z;
    }

    Vector3 Cross(const Vector3& other) {
        return Vector3(
            this->y * other.z - this->z * other.y,
            this->z * other.x - this->x * other.z,
            this->x * other.y - this->y * other.x
        );
    }

    void Rotate(const Angle3& angle, const Vector3& center = Vector3()) {

        Vector3 newPoint = *this - center;

        float cosa = cosf(angle.z.AsRadians());
        float sina = sinf(angle.z.AsRadians());

        float cosb = cosf(angle.y.AsRadians());
        float sinb = sinf(angle.y.AsRadians());

        float cosc = cosf(angle.x.AsRadians());
        float sinc = sinf(angle.x.AsRadians());

        float Axx = cosa * cosb;
        float Axy = cosa * sinb * sinc - sina * cosc;
        float Axz = cosa * sinb * cosc + sina * sinc;

        float Ayx = sina * cosb;
        float Ayy = sina * sinb * sinc + cosa * cosc;
        float Ayz = sina * sinb * cosc - cosa * sinc;

        float Azx = -sinb;
        float Azy = cosb * sinc;
        float Azz = cosb * cosc;

        float px = newPoint.x;
        float py = newPoint.y;
        float pz = newPoint.z;

        newPoint.x = Axx * px + Axy * py + Axz * pz;
        newPoint.y = Ayx * px + Ayy * py + Ayz * pz;
        newPoint.z = Azx * px + Azy * py + Azz * pz;

        *this = newPoint + center;
    }

    void Lerp(const Vector3& other, float amount) {
        *this += (other - *this) * amount;
    }

    void Cerp(const Vector3& other, float amount) {
        Lerp(other, 1.f - (amount - 1.f) * (amount - 1.f));
    }

    float Magnitude() const {
        return sqrtf(Dot(*this));
    }

    void Normalize() {
        *this / Magnitude();
    }

    float x, y, z;
private:

};

class Vector2
{
public:
    constexpr Vector2() : x(0), y(0) {}

    constexpr Vector2(float xVal, float yVal) : x(xVal), y(yVal) {}

    float x, y;
private:
    void operator=(const Vector2& right)
    {
        this->x = right.x;
        this->y = right.y;
    }

    Vector2 operator+(const Vector2& right)
    {
        Vector2 ret;
        ret.x = this->x + right.x;
        ret.y = this->y + right.y;
        return ret;
    }

    void operator+=(const Vector2& right)
    {
        this->x += right.x;
        this->y += right.y;
    }

    Vector2 operator-(const Vector2& right)
    {
        Vector2 ret;
        ret.x = this->x - right.x;
        ret.y = this->y - right.y;
        return ret;
    }

    void operator-=(const Vector2& right)
    {
        this->x -= right.x;
        this->y -= right.y;
    }

    Vector2 operator*(const float& amount)
    {
        Vector2 ret;
        ret.x = this->x * amount;
        ret.y = this->y * amount;
        return ret;
    }

    void operator*=(const float& amount)
    {
        this->x *= amount;
        this->y *= amount;
    }

    Vector2 operator/(const float& amount)
    {
        Vector2 ret;
        ret.x = this->x * amount;
        ret.y = this->y * amount;
        return ret;
    }

    void operator/=(const float& amount)
    {
        this->x /= amount;
        this->y /= amount;
    }
};