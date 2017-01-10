/*

    Requires the Qt widget libraries, available at no cost at
    http://www.troll.no

    Copyright (C) 1997 Peter Putzer
                       putzer@kde.org

    This program is free software; you can redistribute it and/or modify
    it under the terms of version 2 of the GNU General Public License
    as published by the Free Software Foundation.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

// KSysV Config Object

#include <kapp.h>
#include "ksv_conf.h"

// include meta-object code
#include "ksv_conf.moc"


KSVConfig::KSVConfig() : QObject(0, "KSVConfigObject")
{
  conf = kapp->getConfig();
  readConfig();
}

// Destructor
KSVConfig::~KSVConfig() {
  // not possible since this is a global object
  //   writeConfig();
}

void KSVConfig::readConfig() {
  conf->setGroup("Path Settings");

  ScriptPath = conf->readEntry("ScriptPath", "/etc/rc.d/init.d");
  RLPath = conf->readEntry("RunlevelPath", "/etc/rc.d");

  conf->setGroup("Other Settings");

  showLog = conf->readBoolEntry("ShowLog", TRUE);
  showStatus = conf->readBoolEntry("StatusBar", TRUE);
  showTools = conf->readBoolEntry("ToolBar", TRUE);
  configured = conf->readBoolEntry("Configured", FALSE);
  _panning = conf->readNumEntry("PanningFactor", 80);

  conf->setGroup("Colors");
  _newColor = conf->readColorEntry("New", &blue);
  _changedColor = conf->readColorEntry("Changed", &red);
}

void KSVConfig::writeConfig() {
  conf->setGroup("Path Settings");	
  conf->writeEntry("ScriptPath", ScriptPath);
  conf->writeEntry("RunlevelPath", RLPath);

  conf->setGroup("Other Settings");
  conf->writeEntry("ShowLog", showLog);
  conf->writeEntry("StatusBar", showStatus);
  conf->writeEntry("ToolBar", showTools);
  conf->writeEntry("Configured", configured);
  conf->writeEntry("PanningFactor", _panning);

  conf->setGroup("Colors");
  conf->writeEntry("New", _newColor);
  conf->writeEntry("Changed", _changedColor);

  // save screen geometry
  conf->setGroup(QString("Geometry"));
  conf->writeEntry(QString("X-Position"), kapp->mainWidget()->x());
  conf->writeEntry(QString("Y-Position"), kapp->mainWidget()->y());
  conf->writeEntry(QString("Width"), kapp->mainWidget()->width());
  conf->writeEntry(QString("Height"), kapp->mainWidget()->height());

  // flush everything
  conf->sync();
}

void KSVConfig::cancelConfig() {
  // maybe that's the wrong place for this slot
}

void KSVConfig::setShowLog( bool val ) {
  showLog = val;
}

void KSVConfig::setShowStatus( bool val ) {
  showStatus = val;
}

void KSVConfig::setShowTools( bool val ) {
  showTools = val;
}

void KSVConfig::setScriptPath( QString path ) {
  ScriptPath = path;
}

void KSVConfig::setRLPath( QString path ) {
  RLPath = path;
}

const bool KSVConfig::getShowLog() const {
  return showLog;
}

const bool KSVConfig::getShowTools() const {
  return showTools;
}

const bool KSVConfig::getShowStatus() const {
  return showStatus;
}

const QString KSVConfig::getScriptPath() const {
  return ScriptPath;
}

const QString KSVConfig::getRLPath() const {
  return RLPath;
}

const QPoint KSVConfig::getPosition() const {
  conf->setGroup("Geometry");

  return QPoint(conf->readNumEntry("X-Position", 0),
		conf->readNumEntry("Y-Position", 0));
}

const QSize KSVConfig::getSize() const {
  conf->setGroup("Geometry");

  return QSize(conf->readNumEntry("Width", 600),
	       conf->readNumEntry("Height", 400));
}

void KSVConfig::setConfigured( bool val ) {
  configured = val;
}

const bool KSVConfig::isConfigured() const {
  return configured;
}

const int KSVConfig::getPanningFactor() const {
  return _panning;
}

void KSVConfig::setPanningFactor(const int val) {
  int p = val > 100 ? 100 : val;
  p = p < 0 ? 0 : p;

  _panning = p;
}

void KSVConfig::setNewColor( const QColor& color ) {
  _newColor = color;
}

void KSVConfig::setChangedColor( const QColor& color ) {
  _changedColor = color;
}

const QColor& KSVConfig::getNewColor() const {
  return _newColor;
}

const QColor& KSVConfig::getChangedColor() const {
  return _changedColor;
}
