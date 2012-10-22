#include "sniffersettings.h"
#include <QtGui>
#include <QSettings>

SnifferSettings::SnifferSettings(QWidget* parent): QWidget(parent), ui(new Ui::SnifferSettings){

}

QWidget* SnifferSettings::getWidget(){}

void SnifferSettings::setCurrentSettings(){}

void SnifferSettings::setDefaultSettings(){}

void SnifferSettings::applySettings(){}
