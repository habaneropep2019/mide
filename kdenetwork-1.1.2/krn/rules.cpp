#include "rules.h"
#include "NNTP.h"

#include <qlist.h>
#include <ksimpleconfig.h>

extern QList <Rule> ruleList;
extern KSimpleConfig *ruleFile;


Rule::Rule(const char *_name=0,const char *expr=0,Field _field,
           bool casesen,bool wildmode,int _value=0)
{
    regex=QRegExp(expr,casesen,wildmode);
    name=_name;
    field=_field;
    value=_value;
}

Rule::Rule(const char *_name=0,const char *expr=0,const char *_field,
           bool casesen,bool wildmode,int _value=0)
{
    regex=QRegExp(expr,casesen,wildmode);
    name=_name;
    if (!strcmp(_field,"Sender"))
        field=Sender;
    else if (!strcmp(_field,"Subject"))
        field=Subject;
    else if (!strcmp(_field,"Cached Header"))
        field=Header;
    else if (!strcmp(_field,"Header"))
        field=Header2;
    else if (!strcmp(_field,"Cached Body"))
        field=Body;
    else if (!strcmp(_field,"Body"))
        field=Body2;
    else if (!strcmp(_field,"Cached Article"))
        field=All;
    else if (!strcmp(_field,"Article"))
        field=All2;
    value=_value;
}

Rule::~Rule()
{
}


int Rule::missingParts()
{
    int parts=PART_NONE;

    if (field==Header2)
        parts=parts|PART_HEAD;
    else if (field==Body2)
        parts=parts|PART_BODY;
    else if (field==All2)
        parts=parts|PART_ALL;
    return parts;
}


bool Rule::match(const Article art,NNTP *server)
{
    bool matches=false;
    if (server)
        server->getMissingParts((MessageParts)missingParts(),art.ID);
    
    if (field==Subject)
    {
        if (regex.match(art.Subject.data())>-1)
        {
            matches=true;
        }
    }
    else if (field==Sender)
    {
        if (regex.match(art.From.data())>-1)
        {
            matches=true;
        }
    }

    if (!server)
        return matches;
    
    else if ((field==Header && (server->isCached(art.ID)&PART_HEAD)) ||
            (field==Header2))
    {
        QString *data=server->head(art.ID);
        if (regex.match(data->data())>-1)
        {
            matches=true;
        }
    }

    else if ((field==Body && (server->isCached(art.ID)&PART_BODY)) ||
            (field==Body2))
    {
        QString *data=server->body(art.ID);
        if (regex.match(data->data())>-1)
        {
            matches=true;
        }
    }

    else if ((field==All && (server->isCached(art.ID)&PART_ALL)) ||
            (field==All2))
    {
        QString *data=server->article(art.ID);
        if (regex.match(data->data())>-1)
        {
            matches=true;
        }
    }
    
    return matches;
}

void Rule::load(const char *name)
{
    ruleFile->setGroup(name);
    regex=QRegExp(ruleFile->readEntry("Regexp",""),
                  ruleFile->readNumEntry("CaseSensitive",false),
                  ruleFile->readNumEntry("WildCard",false));
    value=ruleFile->readNumEntry("Value",0);
    field=(Rule::Field)(ruleFile->readNumEntry("Field",0));
}
void Rule::save(const char *name)
{
    ruleFile->setGroup("Index");
    QStrList names;
    ruleFile->readListEntry("RuleNames",names);
    if (names.find(name)==-1)
    {
        names.append(name);
        ruleFile->writeEntry("RuleNames",names);
    }
    ruleFile->setGroup(name);
    ruleFile->writeEntry("Name",name);
    ruleFile->writeEntry("Regexp",regex.pattern());
    ruleFile->writeEntry("Field",field);
    ruleFile->writeEntry("WildCard",regex.wildcard());
    ruleFile->writeEntry("CaseSensitive",regex.caseSensitive());
    ruleFile->writeEntry("Value",value);
    ruleFile->sync();
}

void Rule::updateGlobals()
{
    ruleList.clear();
    ruleFile->setGroup("Index");
    QStrList names;
    ruleFile->readListEntry("GlobalRules",names);
    for (char *iter=names.first();iter!=0;iter=names.next())
    {
        Rule *r=new Rule(0,0,Rule::Sender,false,false);
        r->load(iter);
        ruleList.append(r);
    }
}
