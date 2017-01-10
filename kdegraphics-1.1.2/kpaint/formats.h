// -*- c++ -*-

#ifndef FORMATS_H
#define FORMATS_H

#include <qimage.h>
#include <qlist.h>
#include <qstrlist.h>

/**
 * This file defines the format records for the supported file formats
 * and the manager to install them
 */
class FormatRecord {
public:
  /**
   * Constants for flags
   */
  enum {InternalFormat = 1, ReadFormat = 2, WriteFormat = 4};
  
  const char *formatName;
  /**
   * Internal? / Read? / Write?
   */
  unsigned int flags;
  /**
   * 0 for formats kpaint should not register
   */ 
  char *magic;
  char *glob;
  /**
   * Default suffix for this type
   */
  char *suffix;
  /**
   * 0 for internal formats
   */
  image_io_handler read_format;
  /**
   * 0 for internal formats
   */
  image_io_handler write_format;

};

class FormatManager {
 public:
   FormatManager();
   virtual ~FormatManager();
   
   const char *allImagesGlob();
   QStrList *formats();
   const char *glob(const char *format);
   const char *suffix(const char *format);
   // const char *description(const char *format);
   // const unsigned int flags(const char *format);
   
 protected:
   QList <FormatRecord> list;
   virtual void init(FormatRecord formatlist[]);
   QStrList names;
   QString globAll;
};

#endif // FORMATS_H
