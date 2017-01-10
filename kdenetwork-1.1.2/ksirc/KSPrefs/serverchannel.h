/**********************************************************************

	--- Qt Architect generated file ---

	File: serverchannel.h
	Last generated: Sat Jan 17 14:21:43 1998

 *********************************************************************/

#ifndef ServerChannel_included
#define ServerChannel_included

#include "serverchanneldata.h"

class ServerChannel : public serverchanneldata
{
    Q_OBJECT

public:

    ServerChannel
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

    virtual ~ServerChannel();

public slots:
    void slot_apply();
    void servers_delete();
    void servers_add();
    void channels_add();
    void channels_delete();

    void servers_sle_update(const char *);
    void channels_sle_update(const char *);
     

};
#endif // ServerChannel_included
