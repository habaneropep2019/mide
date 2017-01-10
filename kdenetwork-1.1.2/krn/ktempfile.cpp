#include "ktempfile.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

KTempFile::KTempFile()
          :QCollection()
{
    char* tmp=tmpnam(NULL);
    mdir=tmp;
    mkdir(mdir,S_IRWXU);
    madel=TRUE;
    files.setAutoDelete(TRUE);
    infos.setAutoDelete(TRUE);
    mcount=0;
}

KTempFile::~KTempFile()
{
    clear();
    rmdir(mdir);
}

QString KTempFile::dir()
{
    return mdir;
}

bool KTempFile::autoDelete()
{
    return madel;
}

void KTempFile::setAutoDelete(bool s)
{
    madel=s;
}

int KTempFile::create(QString prefix, QString suffix)
{
    QString num;
    num.setNum(mcount++);
    if(QFile::exists(mdir+'/'+prefix+num+suffix))
        return create(mdir+'/'+prefix,suffix);

    QFile* f=new QFile(mdir+'/'+prefix+num+suffix);
    QFileInfo* i=new QFileInfo(*f);
    files.append(f);
    infos.append(i);
    debug("n files: %d",files.count());
    return files.count()-1;
}

unsigned int KTempFile::count() const
{
    return mcount;
}

QFile* KTempFile::file(int id)
{
    return files.at(id);
}

QFileInfo* KTempFile::info(int id)
{
    return infos.at(id);
}

void KTempFile::clear()
{
    for(int a=0; a<mcount; a++)
    {
        file(a)->close();
        remove(a);
    }
    for(int a=0; a<mcount; a++)
    {
        files.remove(a);
        infos.remove(a);
    }
}

void KTempFile::remove(int id)
{
    if( QFile::exists(file(id)->name()) ) unlink(file(id)->name());
}
