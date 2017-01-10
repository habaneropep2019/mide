/*
 *   khexdit - a little hex editor
 *   Copyright (C) 1996,97,98  Stephan Kulow
 *
 */

#include "hexdata.h"
#include <qfile.h>
#include <unistd.h>
#include <sys/mman.h>
#include <kapp.h>
#include <qmsgbox.h>

HexData::HexData() {
    hexdata = 0;
    data_size = 0;
}

int HexData::save(const char *filename) {
    QFile file(filename);
    file.open(IO_Truncate | IO_WriteOnly | IO_Raw);
    file.writeBlock(reinterpret_cast<char*>(hexdata), data_size);
    file.close();
    return 0;
}

uchar HexData::byteAt(unsigned long i) const {
    if (i < data_size)
	return hexdata[i]; 
    warning("HexData: index out of range");
    return 0;
}

int HexData::load(const char *Filename) {
    QString fileString(Filename);
    QFile file(fileString);
    if (!file.open(IO_ReadOnly | IO_Raw)) {
	QString txt;
	txt.sprintf(i18n("Error opening %s"),fileString.data());
	QMessageBox::message(i18n("Error"),txt,
			     i18n("Close"));
	return -1;
    }
    hexdata = new uchar[file.size()];
    data_size = file.size();
    uint alr_read = 0;
    while (alr_read < data_size) {
	int ret =  file.readBlock(reinterpret_cast<char*>(hexdata),
				  file.size());
	if (ret < 0) {
	    warning("loading failed");
	    data_size = 0;
	    delete [] hexdata;
	    hexdata = 0;
	}
	alr_read += ret;
    }
    return 0;
}

