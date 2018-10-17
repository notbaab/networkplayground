#ifndef InputStatePacket_h
#define InputStatePacket_h

class InputStatePacket
{
  public:
    InputStatePacket() {}

    InputStatePacket(float xDelta, float yDelta)
    {
        xZero = (xDelta == 0.f);
        xPositive = xDelta > 0.f;

        yZero = (yDelta == 0.f);
        yPositive = yDelta > 0.f;
    }

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        stream.serialize(xZero);
        if (!xZero)
        {
            stream.serialize(xPositive);
        }

        stream.serialize(yZero);
        if (!yZero)
        {
            stream.serialize(yPositive);
        }
        return true;
    }

    float getXDelta() { return getDelta(xZero, xPositive); }
    float getYDelta() { return getDelta(yZero, yPositive); }

  private:
    float getDelta(bool zero, bool positive)
    {
        if (zero)
        {
            return 0.f;
        }

        return positive ? 1.f : -1.f;
    }
    bool xZero, xPositive, yZero, yPositive;
};
#endif /* InputStatePacket_h */
