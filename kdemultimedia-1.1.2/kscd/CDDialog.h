/**********************************************************************

	--- Dlgedit generated file ---

	File: CDDialog.h
	Last generated: Sun Dec 28 19:48:13 1997

 *********************************************************************/

#ifndef CDDialog_included
#define CDDialog_included

#include "CDDialogData.h"
#include <qstrlist.h>
#include <qdatetm.h>
#include <smtpconfig.h>


extern "C" {
#include "struct.h"
}

struct mytoc {
	int	min;
	int	sec;
	int	frame;
        int     absframe;
};


struct dialogcdinfo {

  unsigned long magicID;	/*cddb magic disk id BERND*/
	int	ntracks;	/* Number of tracks on the disc */
	int	length;		/* Total running time in seconds */
        struct mytoc *cddbtoc;
};

class CDDialog : public CDDialogData
{
    Q_OBJECT

public:

    CDDialog
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~CDDialog();


    void setData(
		 struct cdinfo_wm *cd,
		 QStrList& tracktitlelist,
		 QStrList& extlist,
		 QStrList& discidlist,
		 QString& xmcddata,
		 QString& cat,
		 int& revision,
		 QStrList& playlist,
		 QStrList& pathlist,
		 QString& _mailcmd,
                 QString& _submitaddress,
                 SMTPConfig::SMTPConfigData *_smtpConfigData
		 );

    bool checkit();
    void save_cddb_entry(QString& path,bool upload);
    void getCategoryFromPathName(char* pathname, QString& _category);
    
protected:
    void closeEvent(QCloseEvent* e);
    void keyPressEvent(QKeyEvent* e);
public slots:
    void titleselected(int);
    void titlechanged(const char*);
    void trackchanged();
    void save();
    void extITB();
    void extIB();
    void load();
    void upload();
    void ok();
    void play(int i);

signals:

    void cddb_query_signal(bool);
    void dialog_done();
    void play_signal(int i);

private:
    SMTPConfig::SMTPConfigData *smtpConfigData;
    QStrList 	ext_list;
    QStrList 	track_list;
    QStrList 	pathlist;
    QStrList	playlist;
    QString  	xmcd_data;
    QStrList 	discidlist;
    int      	revision;
    QString     submitaddress;
    QString 	category;
    QString	mailcmd;
    QString     playorder;
    struct dialogcdinfo cdinfo;
    bool            messageNotSent;
};
#endif // CDDialog_included
