#pragma once

#include "../AliveLibCommon/Function.hpp"
#include "BaseAliveGameObject.hpp"
#include "../AliveLibCommon/AddPointer.hpp"

class BaseThrowable;

BaseThrowable* Make_Throwable_49AF30(FP xpos, FP ypos, s16 count);

// NOTE: This base type must exist but seems to have been decimated by the compiler, so this contains pure virtuals for
// non common virtuals, and virtuals for common virtuals.
class BaseThrowable : public BaseAliveGameObject
{
public:
    explicit BaseThrowable(s16 resourceArraySize);


    // New virtuals for throwables
    virtual void VThrow(FP velX, FP velY) = 0;
    virtual bool VCanThrow() = 0;
    virtual bool VIsFalling() = 0;
    virtual void VTimeToExplodeRandom() = 0;

    virtual s16 VGetCount();
    virtual void VToDead();
    virtual void VOnPickUpOrSlapped() override;

    using FnTypeMatcher = AddPointer_t<bool(ReliveTypes type)>;
    void BaseAddToPlatform(FnTypeMatcher cb);

protected:
    s16 mBaseThrowableCount = 0;
    s16 mBaseThrowableDead = 0;
};

extern u16 gInfiniteThrowables;
extern const AETypes throwable_types_55FAFC[252];
