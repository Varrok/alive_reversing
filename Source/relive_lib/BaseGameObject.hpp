#pragma once

#include "../AliveLibCommon/Types.hpp"
#include "../AliveLibCommon/BitField.hpp"
#include "DynamicArray.hpp"
#include "../relive_lib/data_conversion/guid.hpp"
#include "../relive_lib/ResourceManagerWrapper.hpp"
#include "../relive_lib/ObjectTypes.hpp"

extern u32 sGnFrame;

struct PrimHeader;

class BaseGameObject
{
public:
    enum Options
    {
        eListAddFailed_Bit1 = 0x001,
    };

    BaseGameObject(s16 bAddToObjectList, s16 resourceArraySize);


    virtual ~BaseGameObject();

    virtual void VUpdate();

    virtual void VRender(PrimHeader** pOrderingTable);

    virtual void VScreenChanged();

    virtual void VStopAudio();

    virtual s32 VGetSaveState(u8* /*pSaveBuffer*/);

    static ReliveTypes FromAO(AO::AOTypes aoType);
    static AO::AOTypes ToAO(ReliveTypes reliveType);

    static ReliveTypes FromAE(AETypes aeType);

    static AETypes ToAE(ReliveTypes reliveType);

    void SetType(ReliveTypes type);

    ReliveTypes Type() const;

protected:
    static Guid RefreshId(const Guid& objectId);

    // Helper to check if a timer has expired
    template <class T>
    static inline bool Expired(const T& value)
    {
        return static_cast<s32>(sGnFrame) > value;
    }

    template <class T>
    static s32 MakeTimer(const T value)
    {
        return static_cast<s32>(sGnFrame) + value;
    }

public:
    void SetUpdateDelay(s32 updateDelay);

    s32 UpdateDelay() const;

    AnimResource& GetAnimRes(AnimId animId);

    PalResource& GetPalRes(PalId palId);

    const bool& GetListAddFailed() const { return mListAddFailed; }
    void SetListAddFailed(bool val) { mListAddFailed = val; }
    const bool& GetUpdatable() const { return mUpdatable; }
    void SetUpdatable(bool val) { mUpdatable = val; }
    const bool& GetDead() const { return mDead; }
    void SetDead(bool val) { mDead = val; }
    const bool& GetDrawable() const { return mDrawable; }
    void SetDrawable(bool val) { mDrawable = val; }
    const bool& GetIsBaseAnimatedWithPhysicsObj() const { return mIsBaseAnimatedWithPhysicsObj; }
    void SetIsBaseAnimatedWithPhysicsObj(bool val) { mIsBaseAnimatedWithPhysicsObj = val; }
    const bool& GetIsBaseAliveGameObject() const { return mIsBaseAliveGameObject; }
    void SetIsBaseAliveGameObject(bool val) { mIsBaseAliveGameObject = val; }
    const bool& GetCanExplode() const { return mCanExplode; }
    void SetCanExplode(bool val) { mCanExplode = val; }
    const bool& GetInteractive() const { return mInteractive; }
    void SetInteractive(bool val) { mInteractive = val; }
    const bool& GetSurviveDeathReset() const { return mSurviveDeathReset; }
    void SetSurviveDeathReset(bool val) { mSurviveDeathReset = val; }
    const bool& GetUpdateDuringCamSwap() const { return mUpdateDuringCamSwap; }
    void SetUpdateDuringCamSwap(bool val) { mUpdateDuringCamSwap = val; }
    const bool& GetCantKill() const { return mCantKill; }
    void SetCantKill(bool val) { mCantKill = val; }

public:
    Guid mBaseGameObjectId;                  // AE
    Guid mBaseGameObjectTlvInfo;                 // AE
    DynamicArrayT<u8*> field_10_resources_array; // AE
    std::vector<AnimResource> mLoadedAnims;
    std::vector<PalResource> mLoadedPals;

    s8 mBaseGameObjectRefCount = 0; // AO

private:
    ReliveTypes mBaseGameObjectTypeId = ReliveTypes::eNone;
    s32 mBaseGameObjectUpdateDelay = 0;

    bool mListAddFailed = false;
    bool mUpdatable = false;
    bool mDead = false;
    bool mDrawable = false;
    bool mIsBaseAnimatedWithPhysicsObj = false;
    bool mIsBaseAliveGameObject = false;
    bool mCanExplode = false;
    bool mInteractive = false;
    bool mSurviveDeathReset = false;
    bool mUpdateDuringCamSwap = false;
    bool mCantKill = false;
};

class IMap& GetMap();

extern DynamicArrayT<BaseGameObject>* gBaseGameObjects;
