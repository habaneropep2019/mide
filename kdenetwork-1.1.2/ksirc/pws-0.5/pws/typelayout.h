#ifndef TYPE_LAYOUT_H
#define TYPE_LAYOUT_H

#include <qwidget.h>
#include <qdict.h>
#include <qstack.h>
#include <qlayout.h>

#include "tlwidget.h"
#include "tlform.h"

class QPixmap;
class TLForm;
class QColor;

/** Class to create dialogs, using typographical layout
 */

class KTypeLayout: QObject
{
public:
    /** Create a KTypeLayout inside of the parent widget
     WARNING: parent's size will probably be affected, since
     it will have to be at least large enough to contain its
     children.
     */
    KTypeLayout(){};
    KTypeLayout(TLForm *parent);
    virtual ~KTypeLayout();
    
    /** No more children will be added, so geometry management can be done
     Unlike the Qt layout managers, you CAN NOT add any more children
     after this, or nasty things will happen.
     */
    void activate();
    
    
    /** Adds a widget to the layout.
     Since KTypeLayout tends to shrink things to its minimum size and
     then stretch them, you should be careful and set minimum/fixed/maximum
     sizes for your widget.
     Use only if none of the convenience addXXX functions works for you.
     Returns a pointer to the TLobj with the widget.
     */
    TLObj *addWidget(const char *ID,QWidget *w);
    
    /** Adds a frame with an optional border and an optional title.
     Until next end() call, widgets will be children of this widget.
     If you specify a frame, it looks like Qt's GroupBox.
     Returns a pointer to the TLForm object that's the new parent.
     Groups should have a corresponding endGroup() call.
     */
     TLForm *addGroup(const char *ID,const char *title="", bool frame=false);

     /** Adds a "book" widget, with no pages.
      Be careful not to add widgets before creating pages,
      because geometry management will go nuts.
      Books should have a corresponding endGroup() call.
     */
     TLBook *addBook(const char *ID);

     /** Adds a page to the current widget, that better be a book, or
      you gonna be on BIIIIIIG trouble.
      Pages should have a corresponding endGroup() call.
      */
     TLForm *addPage(const char *ID,const char *title="");
     
    /** Marks the end of a group (as added by addGroup).
     This make following widgets children of the next outer group.
     If there are no groups to end, it is the same as activate()..
     */
    void endGroup();
    
    /** Adds a label, with a minimum size at least enough for the text to fit
     */
    TLObj *addLabel(const char *ID,const char *text="");

    /** Adds a button, with a minimum size at least enough for the text to fit
     and also at least 75 pixels wide and 30 pixels tall.
     This way you get a button that's as good looking enough as possible.
     These buttons have a fixed size by default.
     */
    TLObj *addButton(const char *ID,const char *text="");

    /** Adds a button with a pixmap instead of text
     */
    TLObj *addButton(const char *ID,const QPixmap &p);

    /** Adds a KColorButton
     */
    TLObj *addColorButton(const char *ID,const QColor *c=0);

    /**
     Adds a Checkbox, with the usual respect for the environment
     */
    TLObj *addCheckBox(const char *ID,const char *text="",bool checked=false);

    /**
     Adds a Radio button. GroupID is the group of buttons that are exclusive
     (only one of them checked at a time).
     The GroupID's are global like the IDs, so be careful not using the
     same in two unrelated places.
     id (lowercase) is the same as in QButtonGroup::insert.
     */
    TLObj *addRadioButton(const char *ID,const char *GroupID,const char *text="",
                          bool checked=false,int id=-1);

    /**
     Adds a Line for data entry ( a QLineEdit).
     text is the text it should contain.
     maxlen is an enforced maximum length. It affects the data entered, not
     the size of the widget, -1 means no limit.
     */
    TLObj *addLineEdit(const char *ID,const char *text="",int maxlen=-1);
    TLObj *addMultiLineEdit(const char *ID,const char *text="",int numrows=-1);

    /**
     Adds a Line for Integer data entry ( a KIntLineEdit).
     text is the text it should contain.
     maxlen is an enforced maximum length. It affects the data entered, not
     the size of the widget, -1 means no limit.
     */
    TLObj *addIntLineEdit(const char *ID,const char *text="",int maxlen=-1);

    /**
     Adds a ListBox.
     contents is a list of textual contents. If you want to set contents
     later, or use pixmaps, just set it to 0, and use findWidget() later.
     minRows is the minimum number of rows that should be visible.
     Think that if you get much below 5, then a ComboBox is probably a
     better widget choice.
     */
     TLObj *addListBox (const char *ID,QStrList *contents=0,int minRows=5);

     /**
      Similar to addListBox, but creates a QComboBox instead of a QListBox
      if rw=TRUE, it's a editable QComboBox
      */
     
     TLObj *addComboBox (const char *ID,QStrList *contents=0,bool rw=FALSE);

    /** Ends a line of widgets, and starts a new one. Think about it
     as pressing "return" in a word processor
     */
    void newLine();

    /** Skips a cell horizontally. Since widgets are loosely arranged
     in a grid, you can use this command to leave a cell apparently empty.
     Keep in mind that this won't affect that columns' minimum width at all.
     */
    void skip();

    /** Sets an object's alignment within his cell
     It's like the alignment in Qt's QGridLayout::addWidget, except
     that an alignment value of AlignLeft|AlignRight means "justify",
     so the widget will stretch to fill its cell.
     The same is valid for AlignTop|AlignBottom.
     */
    static void setAlign (char *ID,int _align);

    /** Makes a widget spawn vSpawn rows and hSpawn columns in the
     layout grid.
     Obviously, by default widgets occupy only one cell and have both
     values set to 1.
     */
    static void setSpawn (char *ID,int vSpawn=1,int hSpawn=1);

    /** Returns a pointer to the TLObj that matches this ID.
     That ID is stored in a dictionary that is shared by all KTypeLayouts,
     so be careful not to create objects with identical IDs, or one of them
     may become unaccessible!
     However, having inaccessible Labels or groupboxes is usually not
     a big problem.
     You can access the widget as TLObj->widget, etcetera.
     */
    static TLObj *findObject(const char *_ID);

    /* Returns a pointer to the QWidget created by one of the add...
     commands above identified with ID.
     The kind of widget depends basically on how you created it,
     so a cast will most likely be necessary to access some functionality.
     Returns 0 if it can't find ID, so check that pointer!.
     */
    static QWidget *findWidget(const char *_ID);
    
    QList < QList <TLObj> > rows;
    QStack <TLForm> windowStack;
    QGridLayout *grid;
    int outBorder;
    int inBorder;
protected:
    QWidget *widget;
    TLForm *parent;
    TLObj *lastobj;
    QString path;
    virtual void end();
};

/** Special layout for book widgets. It just sets the widget's
 minimum size big enough to hold the largest of its pages.
 */
class KBookLayout: public KTypeLayout
{
public:
    KBookLayout(){};
    KBookLayout(TLForm *parent);
    ~KBookLayout();
private:
    void end();
};

#endif