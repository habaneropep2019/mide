// KDat - a tar-based DAT archiver
// Copyright (C) 1998  Sean Vyain, svyain@mail.tds.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include <stdlib.h>

#include <qchkbox.h>
#include <qcombo.h>
#include <qgrpbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlined.h>
#include <qpushbt.h>

#include <kapp.h>
#include <kfiledialog.h>

#include "Options.h"
#include "OptionsDlg.h"

#include "OptionsDlg.moc"

OptionsDlg::OptionsDlg( QWidget* parent, const char* name = 0 )
        : QDialog( parent, name, TRUE )
{
    setCaption( i18n( "KDat: Options" ) );
    setIconText( i18n( "KDat: Options" ) );

    resize( 300, 300 );

    QLabel* lbl1 = new QLabel( i18n( "Default tape size:" ), this );
    QLabel* lbl2 = new QLabel( i18n( "Tape block size:" ), this );
    QLabel* lbl3 = new QLabel( i18n( "Tape device:" ), this );
    QLabel* lbl4 = new QLabel( i18n( "Tar command:" ), this );

    int max = lbl1->sizeHint().width();
    if ( lbl2->sizeHint().width() > max ) max = lbl2->sizeHint().width();
    if ( lbl3->sizeHint().width() > max ) max = lbl3->sizeHint().width();
    if ( lbl4->sizeHint().width() > max ) max = lbl4->sizeHint().width();

    lbl1->setFixedSize( max, lbl1->sizeHint().height() );
    lbl2->setFixedSize( max, lbl2->sizeHint().height() );
    lbl3->setFixedSize( max, lbl3->sizeHint().height() );
    lbl4->setFixedSize( max, lbl4->sizeHint().height() );

    _defaultTapeSize = new QLineEdit( this );
    _defaultTapeSize->setFixedSize( 48, _defaultTapeSize->sizeHint().height() );

    _defaultTapeSizeUnits = new QComboBox( this );
    _defaultTapeSizeUnits->setFixedSize( 48, _defaultTapeSizeUnits->sizeHint().height() );
    _defaultTapeSizeUnits->insertItem( "MB" );
    _defaultTapeSizeUnits->insertItem( "GB" );

    _tapeBlockSize = new QLineEdit( this );
    _tapeBlockSize->setFixedSize( 48, _tapeBlockSize->sizeHint().height() );

    QLabel* lbl5 = new QLabel( i18n( "bytes" ), this );
    lbl5->setFixedSize( 48, lbl5->sizeHint().height() );

    _tapeDevice = new QLineEdit( this );
    _tapeDevice->setFixedHeight( _tapeDevice->sizeHint().height() );

    QPushButton* browseTapeDevice = new QPushButton( i18n( "..." ), this );
    browseTapeDevice->setFixedSize( browseTapeDevice->sizeHint() );

    _tarCommand = new QLineEdit( this );
    _tarCommand->setFixedHeight( _tarCommand->sizeHint().height() );

    QPushButton* browseTarCommand = new QPushButton( i18n( "..." ), this );
    browseTarCommand->setFixedSize( browseTarCommand->sizeHint() );

    QGroupBox* g1 = new QGroupBox( i18n( "Tape drive options" ), this );

    _loadOnMount = new QCheckBox( i18n( "Load tape on mount" ), g1 );
    _loadOnMount->setFixedHeight( _loadOnMount->sizeHint().height() );

    _lockOnMount = new QCheckBox( i18n( "Lock tape drive on mount" ), g1 );
    _lockOnMount->setFixedHeight( _lockOnMount->sizeHint().height() );

    _ejectOnUnmount = new QCheckBox( i18n( "Eject tape on unmount" ), g1 );
    _ejectOnUnmount->setFixedHeight( _ejectOnUnmount->sizeHint().height() );

    _variableBlockSize = new QCheckBox( i18n( "Variable block size" ), g1 );
    _variableBlockSize->setFixedHeight( _variableBlockSize->sizeHint().height() );

    g1->setMinimumHeight( 8 + 8 + _loadOnMount->sizeHint().height() + 4 + _lockOnMount->sizeHint().height() + 4 + _ejectOnUnmount->sizeHint().height() + 4 + _variableBlockSize->sizeHint().height() + 8 );

    QPushButton* ok     = new QPushButton( i18n( "OK" )    , this );
    QPushButton* apply  = new QPushButton( i18n( "Apply" ) , this );
    QPushButton* cancel = new QPushButton( i18n( "Cancel" ), this );

    ok->setFixedSize( 80, ok->sizeHint().height() );
    apply->setFixedSize( 80, apply->sizeHint().height() );
    cancel->setFixedSize( 80, cancel->sizeHint().height() );

    connect( browseTapeDevice, SIGNAL( clicked() ), this, SLOT( slotBrowseTapeDevice() ) );
    connect( browseTarCommand, SIGNAL( clicked() ), this, SLOT( slotBrowseTarCommand() ) );

    connect( ok    , SIGNAL( clicked() ), this, SLOT( slotOK()     ) );
    connect( apply , SIGNAL( clicked() ), this, SLOT( slotApply()  ) );
    connect( cancel, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );

    QVBoxLayout* l1 = new QVBoxLayout( this, 8, 4 );

    QHBoxLayout* l1_1 = new QHBoxLayout();
    l1->addLayout( l1_1 );
    l1_1->addWidget( lbl1 );
    l1_1->addWidget( _defaultTapeSize );
    l1_1->addWidget( _defaultTapeSizeUnits );

    QHBoxLayout* l1_2 = new QHBoxLayout();
    l1->addLayout( l1_2 );
    l1_2->addWidget( lbl2 );
    l1_2->addWidget( _tapeBlockSize );
    l1_2->addWidget( lbl5 );

    QHBoxLayout* l1_4 = new QHBoxLayout();
    l1->addLayout( l1_4 );
    l1_4->addWidget( lbl3 );
    l1_4->addWidget( _tapeDevice, 1 );
    l1_4->addWidget( browseTapeDevice );

    QHBoxLayout* l1_5 = new QHBoxLayout();
    l1->addLayout( l1_5 );
    l1_5->addWidget( lbl4 );
    l1_5->addWidget( _tarCommand, 1 );
    l1_5->addWidget( browseTarCommand );

    l1->addWidget( g1 );

    QVBoxLayout* l2 = new QVBoxLayout( g1, 8, 4 );
    l2->addSpacing( 8 );
    l2->addWidget( _loadOnMount );
    l2->addWidget( _lockOnMount );
    l2->addWidget( _ejectOnUnmount );
    l2->addWidget( _variableBlockSize );

    l1->addStretch( 1 );

    QHBoxLayout* l1_6 = new QHBoxLayout();
    l1->addLayout( l1_6 );
    l1_6->addStretch( 1 );
    l1_6->addWidget( ok );
    l1_6->addWidget( apply );
    l1_6->addWidget( cancel );

    int size = Options::instance()->getDefaultTapeSize();
    if ( ( size >= 1024*1024 ) && ( size % ( 1024*1024 ) == 0 ) ) {
        // GB
        size /= 1024*1024;
        _defaultTapeSizeUnits->setCurrentItem( 1 );
    } else {
        // MB
        size /= 1024;
        _defaultTapeSizeUnits->setCurrentItem( 0 );
    }
    QString tmp;
    tmp.setNum( size );
    _defaultTapeSize->setText( tmp );

    tmp.setNum( Options::instance()->getTapeBlockSize() );
    _tapeBlockSize->setText( tmp );

    _tapeDevice->setText( Options::instance()->getTapeDevice() );

    _tarCommand->setText( Options::instance()->getTarCommand() );

    _loadOnMount->setChecked( Options::instance()->getLoadOnMount() );

    _lockOnMount->setChecked( Options::instance()->getLockOnMount() );

    _ejectOnUnmount->setChecked( Options::instance()->getEjectOnUnmount() );

    _variableBlockSize->setChecked( Options::instance()->getVariableBlockSize() );
}

OptionsDlg::~OptionsDlg()
{
}

void OptionsDlg::slotBrowseTapeDevice()
{
    QString tmp;
    tmp = KFileDialog::getOpenFileName( _tapeDevice->text() );
    if ( tmp.length() ) {
        _tapeDevice->setText( tmp );
    }
}

void OptionsDlg::slotBrowseTarCommand()
{
    QString tmp;
    tmp = KFileDialog::getOpenFileName( _tarCommand->text() );
    if ( tmp.length() ) {
        _tarCommand->setText( tmp );
    }
}

void OptionsDlg::slotOK()
{
    slotApply();
    accept();
}

void OptionsDlg::slotApply()
{
    int size = atoi( _defaultTapeSize->text() );
    if ( _defaultTapeSizeUnits->currentItem() == 0 ) {
        // MB
        size *= 1024;
    } else {
        // GB
        size *= 1024*1024;
    }
    Options::instance()->setDefaultTapeSize( size );

    Options::instance()->setTapeBlockSize( atoi( _tapeBlockSize->text() ) );

    Options::instance()->setTapeDevice( _tapeDevice->text() );

    Options::instance()->setTarCommand( _tarCommand->text() );

    Options::instance()->setLoadOnMount( _loadOnMount->isChecked() );

    Options::instance()->setLockOnMount( _lockOnMount->isChecked() );

    Options::instance()->setEjectOnUnmount( _ejectOnUnmount->isChecked() );

    Options::instance()->setVariableBlockSize( _variableBlockSize->isChecked() );

    Options::instance()->sync();
}

void OptionsDlg::slotCancel()
{
    reject();
}
