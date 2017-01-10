/**********************************************************************

	--- Qt Architect generated file ---

	File: FilterRuleEditor.h
	Last generated: Mon Dec 15 18:14:27 1997

 *********************************************************************/

#ifndef FilterRuleEditor_included
#define FilterRuleEditor_included

#include "FilterRuleEditorData.h"
#include <qstring.h>

class FilterRuleEditor : public FilterRuleEditorData
{
    Q_OBJECT

public:

    FilterRuleEditor
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~FilterRuleEditor();

protected slots:
     virtual void newRule();
     virtual void OkPressed();
     virtual void deleteRule();
     virtual void closePressed();
     virtual void newHighlight(int);
     virtual void raiseRule();
     virtual void lowerRule();

protected:
     void updateListBox(int citem = 0);
     void moveRule(int, int);
     QString convertSpecial(QString);
};
#endif // FilterRuleEditor_included
