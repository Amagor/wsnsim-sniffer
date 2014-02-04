#include "statictools.h"
#include "iclientrealtimesettings.h"
#include <QDir>
#include <QLibrary>
#include <QApplication>

IClientRealTimeSettings *StaticTools::getClientRealTimeSettings()
{
    return new ClientRealTimeSettings();
}
