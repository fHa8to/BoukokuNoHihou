#include "Pad.h"
#include "DxLib.h"

namespace
{
    int lastPad = 0;
    int nowPad = 0;
}


namespace Pad
{
    void Pad::Update()
    {
        lastPad = nowPad;

        nowPad = GetJoypadInputState(DX_INPUT_KEY_PAD1);
    }

    bool Pad::IsPress(int key)
    {
        return (nowPad & key);
    }

    bool Pad::IsTrigger(int key)
    {
        bool isNow = (nowPad & key);        //このフレーム
        bool isLast = (lastPad & key);      //前のフレーム

        if (isNow && !isLast)
        {
            return true;
        }
        return false;
    }

    bool Pad::IsRelase(int key)
    {
        bool isNow = (nowPad & key);        //このフレーム
        bool isLast = (lastPad & key);      //前のフレーム

        if (!isNow && isLast)
        {
            return true;
        }
        return false;
    }
}
