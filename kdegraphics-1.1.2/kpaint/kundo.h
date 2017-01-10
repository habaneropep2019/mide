// -*- c++ -*-

// $Id: kundo.h,v 1.2 1997/08/30 18:22:46 kdecvs Exp $

#ifndef KUNDO_H
#define KUNDO_H

#include <qlist.h>

/**
 You must subclass this class to record your undo data. You can override the
 size method to allow the KUndo class size limit to operate by size, by default
 it operates by the number of items in the undo list. Overriding the tag method
 allows you to store more than one type of undo data in the list and identify
 them to check your downcast is ok. 
 */
class KUndoData {
public:
  virtual const char *tag() { return NULL; };
  virtual int size() { return 1; };
};

/**
 This class manages the undo list for an application

 NB size is only per item for now
 */
class KUndo {
public:
  KUndo(int maxsize_= 10);
  ~KUndo();

  // User actions
  void record(KUndoData *);
  KUndoData *undo();
  KUndoData *redo();
  void clear();

  // Can we undo/redo from here?
  bool canUndo() { if (behind > 0) return TRUE; else return FALSE; };
  bool canRedo() { if (infront < 0) return TRUE; else return FALSE; };

  // Handle the size limits
  void setMaxSize(int maxsize_) { maxsize= maxsize_; checksize(); };
  int maxSize() { return maxsize; };

protected:
  // Checks the size is ok and trims the list if it's not
  virtual void checksize();
  QList <KUndoData> undolist;

private:
  int behind;
  int infront;
  int maxsize;
};

#endif // KUNDO_H


