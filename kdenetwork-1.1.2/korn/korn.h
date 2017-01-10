#ifndef SSK_KORN_H
#define SSK_KORN_H

#include<qframe.h>
#include<qlist.h>

class KornSettings;
class QPopupMenu;
class KornOptions;

class KornBox {
public:
	KornBox(){};
	KornBox(KornBox *);

	QString name;
	QString caption;
	QString path;
	int	poll;
	QString notify;
	QString click;
};

class Korn : public QFrame
{
	Q_OBJECT

private:
	QList<KornBox>	*options;
	QPopupMenu	*menu;
	KornOptions	*_optionsDialog;

	/// updates the korn configuration file using the box list
	static int writeOut(QList<KornBox> *);

	/** makes dest an identical copy of src, including deep
	 * copies of the list items.
	 */
	static void cloneBoxList( QList<KornBox> *dest, 
		QList<KornBox> *src);

	/// set by the constructor if korn was started without valid config
	bool _noConfig;
public:
	///
	Korn(QWidget *parent, KornSettings *settings, WFlags f=0);
	///
	bool hasConfig() const { return !_noConfig; };

	~Korn();

/**@name slots*/
//@{
public slots:
	///
	void popupMenu();
	///
	void optionDlg();
	///
	void help();
	///
	void about();
	///
	void dialogClosed( int );
//@}
protected:
	virtual void moveEvent(QMoveEvent *);

};

#endif
