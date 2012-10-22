#ifndef SNIFFERSETTINGS_H
#define SNIFFERSETTINGS_H

#include <QWidget>
#include "interfaces/isniffersettings.h"
#include "ui_sniffersettings.h"

namespace Ui{
    class SnifferSettings;
}

class SnifferSettings : public QWidget, ISnifferSettings
{
    Q_OBJECT
public:
    SnifferSettings(QWidget* parent = 0);
    QWidget* getWidget();
    void setCurrentSettings();
    void setDefaultSettings();
    void applySettings();
private:
    Ui::SnifferSettings *ui;
};

#endif // SNIFFERSETTINGS_H
