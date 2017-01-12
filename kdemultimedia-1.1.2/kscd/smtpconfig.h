/*
 * Kscd - A simple cd player for the KDE Project
 *
 * $Id$
 *
 * Copyright (c) 1997 Bernd Johannes wuebben@math.cornell.edu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef SMTPCONFIG_H
#define SMTPCONFIG_H

#include <qdialog.h>

#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlabel.h>

class SMTPConfig:public QDialog
{
    Q_OBJECT
public:
    struct SMTPConfigData{
        bool enabled;
        QString serverHost;
        QString serverPort;
        QString senderAddress;
    };

    SMTPConfig(QWidget *parent = NULL, const char *name = NULL, struct SMTPConfigData *_configData = NULL);
    ~SMTPConfig() {};

public slots:
    void commitData();
    void enableClicked();
    
signals:

protected:
    QGroupBox       *mainBox;
    QCheckBox       *enableCB;
    QLabel          *serverHostLabel;
    QLineEdit       *serverHostEdit;
    QLabel          *serverPortLabel;
    QLineEdit       *serverPortEdit;
    QLabel          *senderAddressLabel;
    QLineEdit       *senderAddressEdit;

private:
    SMTPConfigData *configData;
};
#endif
