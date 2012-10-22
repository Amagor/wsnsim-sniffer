#ifndef STATICTOOLS_H
#define STATICTOOLS_H

#include <QDir>
#include <QLibrary>
#include <QApplication>

#include "iclientrealtimesettings.h"

class StaticTools
{
public:
    static IClientRealTimeSettings *getClientRealTimeSettings();
};

#endif // STATICTOOLS_H
