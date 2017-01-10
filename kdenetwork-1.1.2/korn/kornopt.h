/**********************************************************************

	--- Dlgedit generated file ---

	File: kornopt.h
	Last generated: Mon Jun 2 20:20:22 1997

 *********************************************************************/

#ifndef KornOptions_included
#define KornOptions_included

#include<qlist.h>

#include "optdata.h"
#include "korn.h"

class KornOptions : public KornOptData
{
    Q_OBJECT

private:
	QList<KornBox> *_list;
	bool _highlighted;

	void updateList();
public:

	KornOptions ( QWidget* parent = NULL, const char* name = NULL);
	virtual ~KornOptions();

	/** Sets the box list. 
	* The class can read or write to this list, but deleting it is 
	* the job of the caller. This class will not delete the list.
	*/
	void setList(QList<KornBox> *);
	/** Return a pointer to the list that the object is currently
	* using. This is the same pointer that was passed to the object
	* with setList.
	* @see setList
	*/
	QList<KornBox> *list() const { return _list; };

protected slots:

	virtual void cancelClicked();
	virtual void boxHighlight(int);
	virtual void boxSelect(int);
	virtual void newBox();
	virtual void okClicked();
	virtual void deleteBox();
	virtual void dataChanged();

signals:
	/// raised when the widget is hidden
	void finished(int r);

};
#endif // KornOptions_included
