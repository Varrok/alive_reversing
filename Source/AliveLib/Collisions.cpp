#include "stdafx.h"
#include "Collisions.hpp"
#include "Function.hpp"
#include "stdlib.hpp"
#include "PathData.hpp"
#include "Psx.hpp"
#include "DebugHelpers.hpp"

ALIVE_VAR(1, 0x5C1128, Collisions*, sCollisions_DArray_5C1128, nullptr);

Collisions* Collisions::ctor_418930(const CollisionInfo* pCollisionInfo, const BYTE* pPathRes)
{
    field_8_item_count = pCollisionInfo->field_10_num_collision_items;
    field_4_current_item_count = static_cast<WORD>(pCollisionInfo->field_10_num_collision_items);

    // Up to 40 dynamic collisions, slam doors, trap doors, lift platforms etc.
    field_C_max_count = pCollisionInfo->field_10_num_collision_items + 40;

    // Allocate memory for collisions array
    field_0_pArray = reinterpret_cast<PathLine*>(malloc_non_zero_4954F0(field_C_max_count * sizeof(PathLine)));

    // Copy collision line data out of Path resource
    memcpy(field_0_pArray, &pPathRes[pCollisionInfo->field_C_collision_offset], field_4_current_item_count * sizeof(PathLine));

    // Init dynamic collisions positions to zeros
    for (int i = field_4_current_item_count; i < field_C_max_count; i++)
    {
        field_0_pArray[i].field_0_x1 = 0;
        field_0_pArray[i].field_2_y1 = 0;
        field_0_pArray[i].field_4_x2 = 0;
        field_0_pArray[i].field_6_y2 = 0;
    }
    return this;
}

void Collisions::dtor_4189F0()
{
    Mem_Free_495560(field_0_pArray);
}

void CC Collisions::Factory_4188A0(const CollisionInfo* pCollisionInfo, const BYTE* pPathRes)
{
    sCollisions_DArray_5C1128 = alive_new<Collisions>();
    if (sCollisions_DArray_5C1128)
    {
        sCollisions_DArray_5C1128->ctor_418930(pCollisionInfo, pPathRes);
    }
}

// 24:8 fixed type.. I guess 16:16 wasn't good enough for collision detection
class Fixed_24_8
{
public:
    Fixed_24_8()
        : fpValue(0)
    {

    }

    explicit Fixed_24_8(int val)
    {
        fpValue = val * 256;
    }

    explicit Fixed_24_8(FP fixed16_16)
    {
        fpValue = fixed16_16.fpValue / 256;
    }

    int GetExponent() const
    {
        return fpValue / 256;
    }

    Fixed_24_8 Abs() const
    {
        Fixed_24_8 r;
        if (fpValue > 0)
        {
            r.fpValue = fpValue;
        }
        else
        {
            r.fpValue = -fpValue;
        }
        return r;
    }

    int fpValue;
};

inline bool operator > (const Fixed_24_8& lhs, const Fixed_24_8& rhs)
{
    return lhs.fpValue > rhs.fpValue;
}

inline bool operator < (const Fixed_24_8& lhs, const Fixed_24_8& rhs)
{
    return lhs.fpValue < rhs.fpValue;
}

inline Fixed_24_8 operator - (const Fixed_24_8& lhs, const Fixed_24_8& rhs)
{
    Fixed_24_8 r;
    r.fpValue = lhs.fpValue - rhs.fpValue;
    return r;
}

inline Fixed_24_8 operator + (const Fixed_24_8& lhs, const Fixed_24_8& rhs)
{
    Fixed_24_8 r;
    r.fpValue = lhs.fpValue + rhs.fpValue;
    return r;
}

inline Fixed_24_8 operator * (const Fixed_24_8& lhs, const Fixed_24_8& rhs)
{
    Fixed_24_8 r;
    r.fpValue = lhs.fpValue * (rhs.GetExponent());
    return r;
}

inline Fixed_24_8 operator / (const Fixed_24_8& lhs, const Fixed_24_8& rhs)
{
    Fixed_24_8 r;
    r.fpValue = lhs.fpValue / (rhs.GetExponent());
    return r;
}

EXPORT PSX_RECT* __stdcall Rect_Clear_418040(PSX_RECT* pRect)
{
    if (!pRect)
    {
        return nullptr;
    }
    pRect->x = 0;
    pRect->w = 0;
    pRect->y = 0;
    pRect->h = 0;
    return pRect;
}

signed __int16 Collisions::Raycast_417A60(FP X1_16_16, FP Y1_16_16, FP X2_16_16, FP Y2_16_16, PathLine** ppLine, FP* hitX, FP* hitY, unsigned int modeMask)
{
    // NOTE: The local static k256_dword_5BC034 is omitted since its actually just a constant of 256

    // The following was a huge help in figuring this out:
    // https://stackoverflow.com/questions/35473936/find-whether-two-line-segments-intersect-or-not-in-c

    Fixed_24_8 x1(X1_16_16);
    Fixed_24_8 x2(X2_16_16);
    Fixed_24_8 y1(Y1_16_16);
    Fixed_24_8 y2(Y2_16_16);

    int minX = min(x1, x2).GetExponent();
    int maxX = max(x1, x2).GetExponent();

    int minY = min(y1, y2).GetExponent();
    int maxY = max(y1, y2).GetExponent();

    Fixed_24_8 xDiff = x2 - x1;
    Fixed_24_8 yDiff = y2 - y1;

    Fixed_24_8 nearestMatch(2); // 512

    Fixed_24_8 epslion;
    epslion.fpValue = 256; // 0.99

    PathLine* pNearestMatch = nullptr;

    for (int i = 0; i < field_C_max_count; i++)
    {
        PathLine* pLine = &field_0_pArray[i];
        if (!(1 << (pLine->field_8_type % 32) & modeMask))
        {
            // Not a match on type
            continue;
        }

        if (min(pLine->field_0_x1, pLine->field_4_x2) > maxX)
        {
            continue;
        }

        if (max(pLine->field_0_x1, pLine->field_4_x2) < minX)
        {
            continue;
        }

        if (min(pLine->field_2_y1, pLine->field_6_y2) > maxY)
        {
            continue;
        }

        if (max(pLine->field_2_y1, pLine->field_6_y2) < minY)
        {
            continue;
        }

        Fixed_24_8 xDiffCurrent(pLine->field_4_x2 - pLine->field_0_x1);
        Fixed_24_8 yDiffCurrent(pLine->field_6_y2 - pLine->field_2_y1);

        Fixed_24_8 det = (xDiffCurrent * yDiff) - (xDiff * yDiffCurrent);
        if (det.Abs() < epslion)
        {
            continue;
        }

        Fixed_24_8 unknown1 =
            xDiffCurrent * (Fixed_24_8(pLine->field_2_y1) - y1) -
            yDiffCurrent * (Fixed_24_8(pLine->field_0_x1) - x1);

        if (det > Fixed_24_8(0))
        {
            if (unknown1 < Fixed_24_8(0))
            {
                continue;
            }
            
            if (unknown1 > det)
            {
                continue;
            }
        }
        else
        {
            if (unknown1 > Fixed_24_8(0))
            {
                continue;
            }
            
            if (unknown1 < det)
            {
                continue;
            }
        }

        Fixed_24_8 unknown2 =
            xDiff * (Fixed_24_8(pLine->field_2_y1) - y1) -
            yDiff * (Fixed_24_8(pLine->field_0_x1) - x1);

        if (det > Fixed_24_8(0))
        {
            if (unknown2 < Fixed_24_8(0))
            {
                continue;
            }
            
            if (unknown2 > det)
            {
                continue;
            }
        }
        else
        {
            if (unknown2 > Fixed_24_8(0))
            {
                continue;
            }
            
            if (unknown2 < det)
            {
                continue;
            }
        }

        unknown1 = unknown1 / det;

        if (unknown1 < nearestMatch)
        {
            nearestMatch = unknown1;
            pNearestMatch = pLine;
        }

    }

    if (nearestMatch < Fixed_24_8(2))
    {
        // TODO: Clean up - not sure how this maps on to the operators.. somehow kept getting the wrong results argh! 
        int xh = x1.fpValue + ((xDiff.fpValue * nearestMatch.fpValue) / 256);
        int yh = y1.fpValue + ((yDiff.fpValue * nearestMatch.fpValue) / 256);

        // TODO: Don't know if removing fractional is correct... but seems to fix issue of Abe falling through floor if attempting to hoist twice at the right most wall
        // TODO: Need to get a trace of both this and the real function and check this is correct
        *hitX = FP_NoFractional(FP_FromRaw(xh << 8));
        *hitY = FP_NoFractional(FP_FromRaw(yh << 8));

        *ppLine = pNearestMatch;

#if DEVELOPER_MODE
        DebugAddRaycast({ X1_16_16,Y1_16_16,X2_16_16,Y2_16_16,*hitX,*hitY, *ppLine, modeMask });
#endif

        return TRUE;
    }

    *ppLine = nullptr;

#if DEVELOPER_MODE
    DebugAddRaycast({ X1_16_16,Y1_16_16,X2_16_16,Y2_16_16,*hitX,*hitY, *ppLine, modeMask });
#endif

    return FALSE;
}

PathLine* Collisions::Add_Dynamic_Collision_Line_417FA0(__int16 x1, __int16 y1, __int16 x2, __int16 y2, char mode)
{
    bool freeItemFound = false;
    int idx = field_8_item_count;
    while (idx < field_C_max_count)
    {
        PathLine* pIter = &field_0_pArray[idx];
        if (!pIter->field_0_x1 && !pIter->field_4_x2 && !pIter->field_2_y1 && !pIter->field_6_y2)
        {
            freeItemFound = true;
            break;
        }
        idx++;
    }

    if (!freeItemFound)
    {
        idx--;
    }

    PathLine* pAddedLine = &field_0_pArray[idx];
    pAddedLine->field_0_x1 = x1;
    pAddedLine->field_2_y1 = y1;
    pAddedLine->field_4_x2 = x2;
    pAddedLine->field_6_y2 = y2;
    pAddedLine->field_8_type = mode;
    pAddedLine->field_C_next = -1;
    pAddedLine->field_A_previous = -1;
    return pAddedLine;
}

PathLine* Collisions::Get_Line_At_Idx_418070(__int16 idx)
{
    if (idx == -1)
    {
        return nullptr;
    }
    return &field_0_pArray[idx];
}

const int kNearLineTollerance = 8;

PathLine* Collisions::PreviousLine_4180A0(PathLine* pLine)
{
    if (pLine->field_A_previous != -1)
    {
        return &field_0_pArray[pLine->field_A_previous];
    }

    if (!field_C_max_count)
    {
        return nullptr;
    }

    for (int i = 0; i < field_C_max_count; i++)
    {
        int xDiff = 0;
        if (pLine->field_0_x1 - field_0_pArray[i].field_4_x2 >= 0)
        {
            xDiff = pLine->field_0_x1 - field_0_pArray[i].field_4_x2;
        }
        else
        {
            xDiff = field_0_pArray[i].field_4_x2 - pLine->field_0_x1;
        }

        if (xDiff <= kNearLineTollerance)
        {
            int yDiff = 0;
            if (pLine->field_2_y1 - field_0_pArray[i].field_6_y2 >= 0)
            {
                yDiff = pLine->field_2_y1 - field_0_pArray[i].field_6_y2;
            }
            else
            {
                yDiff = field_0_pArray[i].field_6_y2 - pLine->field_2_y1;
            }

            if (yDiff <= kNearLineTollerance && (1 << field_0_pArray[i].field_8_type % 32) & (1 << pLine->field_8_type % 32))
            {
                return &field_0_pArray[i];
            }
        }
    }
    return nullptr;
}

PathLine* Collisions::NextLine_418180(PathLine* pLine)
{
    if (pLine->field_C_next != -1)
    {
        return &field_0_pArray[pLine->field_C_next];
    }

    if (!field_C_max_count)
    {
        return 0;
    }
    
    for (int i = 0; i < field_C_max_count; i++)
    {
        int xDiff = 0;
        if (pLine->field_4_x2 - field_0_pArray[i].field_0_x1 >= 0)
        {
            xDiff = pLine->field_4_x2 - field_0_pArray[i].field_0_x1;
        }
        else
        {
            xDiff = field_0_pArray[i].field_0_x1 - pLine->field_4_x2;
        }

        if (xDiff <= kNearLineTollerance)
        {
            int yDiff = 0;
            if (pLine->field_6_y2 - field_0_pArray[i].field_2_y1 >= 0)
            {
                yDiff = pLine->field_6_y2 - field_0_pArray[i].field_2_y1;
            }
            else
            {
                yDiff = field_0_pArray[i].field_2_y1 - pLine->field_6_y2;
            }

            if (yDiff <= kNearLineTollerance && (1 << field_0_pArray[i].field_8_type % 32) & (1 << pLine->field_8_type % 32))
            {
                return &field_0_pArray[i];
            }
        }
    }
    return nullptr;
}
