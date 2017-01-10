#ifndef decoderDlg_included
#define decoderDlg_included

#include <qdialog.h>
#include <qlined.h>

#include "ktablistbox.h"


class decoderDlg : public QDialog
{
    Q_OBJECT
        
public:
    
    decoderDlg( const char* name = NULL );
    
    virtual ~decoderDlg();
    KTabListBox *list;
    QLineEdit *destdir;

private slots:
    void actions(int);
    void decodeAll();
    void close();
    
};
#endif // decoderDlg_included
