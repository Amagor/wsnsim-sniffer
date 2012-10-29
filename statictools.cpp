#include "statictools.h"
#include "iclientrealtimesettings.h"
#include <QDir>
#include <QLibrary>
#include <QApplication>




IClientRealTimeSettings *StaticTools::getClientRealTimeSettings()
{
    QDir::setCurrent(QApplication::applicationDirPath());

    QLibrary globalSettings("./globalSettings");

    typedef IClientRealTimeSettings*(*getClientRealTimeSettings) ();
    getClientRealTimeSettings func = (getClientRealTimeSettings) globalSettings.resolve("getClientRealTimeSettings");

    return func();
}
