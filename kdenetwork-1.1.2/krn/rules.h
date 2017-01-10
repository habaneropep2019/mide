#ifndef RULES_H
#define RULES_H

#include <qregexp.h>
#include <qlist.h>

class KConfigBase;
class Article;
class NNTP;

class Rule
{
public:
    enum Field {Sender,Subject,Header,Body,All,Header2,Body2,All2};
    Rule(const char *name=0,const char *expr=0,Field field=Sender,
        bool casesen=false,bool wildmode=false,int value=0);
    Rule(const char *name=0,const char *expr=0,const char *field=0,
         bool casesen=false,bool wildmode=false,int value=0);
    ~Rule();
    static void updateGlobals();

    void load(const char *name);
    void save(const char *name);
    bool match (const Article art,NNTP *server=0);
    int missingParts();
    QRegExp regex;
    Field field;
    QString name;
    int value;
};

typedef QList <Rule> RuleList;

#endif

