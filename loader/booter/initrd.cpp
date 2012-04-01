/**
 * ReaverOS
 * loader/booter/initrd.cpp
 * InitRD routines.
 */

/**
 * Reaver Project OS, Rose License
 *
 * Copyright (C) 2011-2012 Reaver Project Team:
 * 1. Michał "Griwes" Dominiak
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation is required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Michał "Griwes" Dominiak
 *
 **/

#include "initrd.h"
#include "screen.h"

using Screen::bout;
using Screen::nl;

InitRD * InitRDDriver::s_pInitRD;

void InitRDDriver::Parse(InitRD * pInitRD)
{
    if (pInitRD->NumElements == 0)
    {
        *bout << "PANIC: empty InitRD provided!";
        for (;;) ;
    }

    *bout << "Number of files in InitRD: " << pInitRD->NumElements << nl;

    uint32 iYears = pInitRD->Timestamp / 31436000;
    uint32 iLeaps = iYears / 4;
    uint32 iNotLeaps = (1970 + iYears) / 2000;

    uint32 iDaysSinceEpoch = pInitRD->Timestamp / 86400 - iLeaps + iNotLeaps;
    uint32 iDays = iDaysSinceEpoch / 365;

    uint32 aiMonths[12] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };

    const char * asNames[12] = {
        "January", "February", "March", "April", "May", "June", "July",
        "August", "September", "October", "November", "December"
    };

    uint32 iMonth = 1;

    for (uint32 i = 0; i < 12; i++)
    {
        if (aiMonths[i] >= iDays)
        {
            break;
        }

        iDays -= aiMonths[i];
        iMonth++;
    }

    uint32 iSeconds = pInitRD->Timestamp - (iDaysSinceEpoch + iLeaps - iNotLeaps) * 86400;
    uint32 iHours = iSeconds / 3600;
    iSeconds %= 3600;
    uint32 iMinutes = iSeconds / 60;
    iSeconds %= 60;

    *bout << iDays;

    if (iDays == 1 || iDays == 21 || iDays == 31)
    {
        *bout << "st";
    }

    else if (iDays == 2 || iDays == 22)
    {
        *bout << "nd";
    }

    else if (iDays == 3 || iDays == 23)
    {
        *bout << "rd";
    }

    else
    {
        *bout << "th";
    }

    *bout << " of " << asNames[iMonth - 1] << " " << (1970 + iYears - 1) << ", ";
    if (iHours < 10)
    {
        *bout << "0";
    }

    *bout << iHours << ":";

    if (iMinutes < 10)
    {
        *bout << "0";
    }

    *bout << iMinutes << ":";

    if (iSeconds < 10)
    {
        *bout << "0";
    }

    *bout << iSeconds << nl;

    return;
}

bool _strcmp(char * a, char * b)
{
    while (*a != 0)
    {
        if (*a++ != *b++)
        {
            return false;
        }
    }

    if (*a != *b)
    {
        return false;
    }

    return true;
}

File * InitRDDriver::GetFile(char * filename)
{
    for (uint32 i = 0; i < InitRDDriver::s_pInitRD->NumElements; i++)
    {
        
    }
}
