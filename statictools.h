#ifndef STATICTOOLS_H
#define STATICTOOLS_H

#include "iclientrealtimesettings.h"
#include "clientrealtimesettings.h"

class StaticTools
{
public:
    static IClientRealTimeSettings *getClientRealTimeSettings();
};

#endif // STATICTOOLS_H
