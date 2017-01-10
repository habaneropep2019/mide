/*
 *   khexdit - a little hex editor
 *   Copyright (C) 1996,97,98  Stephan Kulow
 *
 */

#include <qstring.h>

class HexData : protected QByteArray {

public:    
    HexData(const char *filename);
    HexData();

    int load(const char *filename);
    int save(const char *filename);

    uchar operator[] (unsigned long i) const { return hexdata[i]; }
    uchar byteAt(unsigned long i) const;

    ulong size() const { return data_size; }
    void changeByte(ulong i, uchar value) { hexdata[i] = value; }
		    
		    
private:
    uchar *hexdata;
    ulong data_size;
};
