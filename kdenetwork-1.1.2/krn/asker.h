//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// This file os part of KRN, a newsreader for the KDE project.              //
// KRN is distributed under the GNU General Public License.                 //
// Read the acompanying file COPYING for more info.                         //
//                                                                          //
// KRN wouldn't be possible without these libraries, whose authors have     //
// made free to use on non-commercial software:                             //
//                                                                          //
// MIME++ by Doug Sauder                                                    //
// Qt     by Troll Tech                                                     //
//                                                                          //
// This file is copyright 1997 by                                           //
// Roberto Alsina <ralsina@unl.edu.ar>                                      //
// Magnus Reftel  <d96reftl@dtek.chalmers.se>                               //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
#ifndef Asker_included
#define Asker_included

#include <qdialog.h>

class QLabel;
class QLineEdit;
class QPushButton;

class Asker : public QDialog
{
    Q_OBJECT
        
public:
    
    Asker (QWidget* parent = NULL, const char* name = NULL);
    
    virtual ~Asker();
    
    QLabel* label;
    QLineEdit* entry;
    QPushButton* b1;
    
    
};
#endif // Asker_included
