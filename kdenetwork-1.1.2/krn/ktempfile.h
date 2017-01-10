#ifndef ktempfile_included
#define ktempfile_included

#include <qlist.h>
#include <qfile.h>
#include <qfileinf.h>
#include <qstring.h>
#include <qcollect.h>

class KTempFile : public QCollection
{
public:
    KTempFile();
    ~KTempFile();
    
    bool autoDelete();
    void setAutoDelete(bool s=FALSE);
    unsigned int count() const;
    void clear();

    int create(QString prefix, QString suffix);
    QFile* file(int id);
    QFileInfo* info(int id);
    
    QString dir();
    void remove(int id);

private:
    QList<QFile> files;
    QList<QFileInfo> infos;
    QString mdir;
    int mcount;
    bool madel;
};

#endif