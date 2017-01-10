{ match ($0,"\".*\"") }
{ if (RSTART>0) print "i18n("substr($0,RSTART,RLENGTH)");" };
