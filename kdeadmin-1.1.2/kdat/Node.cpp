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

#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>

#include <qdir.h>
#include <qpainter.h>

#include <kapp.h>

#include "Archive.h"
#include "BackupProfile.h"
#include "BackupProfileManager.h"
#include "ImageCache.h"
#include "KDat.h"
#include "Node.h"
#include "Range.h"
#include "Tape.h"
#include "TapeManager.h"

#include "Node.moc"

Node::Node( int type, const char* text, const QPixmap& pixmap )
        : KTreeViewItem( text, pixmap ),
          _type( type )
{
}

Node::~Node()
{
}

void Node::insertNode( Node* child )
{
    static Node* lastParent   = 0;
    static uint  lastDirIndex = 0;

    if ( lastParent != this ) {
        // Recompute lastDirIndex.
        for ( lastDirIndex = 0;
              ( lastDirIndex < childCount() ) && ( ((Node*)childAt( lastDirIndex ))->getType() == TapeDirectoryNodeType );
              lastDirIndex++ );
        lastParent = this;
    }

    int min, mid, max, smin, smax;
    if ( ( child->getType() == TapeDirectoryNodeType ) || ( child->getType() == MountedTapeDirectoryNodeType ) ) {
        min = 0;
        max = lastDirIndex - 1;
        lastDirIndex++;
    } else {
        min = lastDirIndex;
        max = childCount() - 1;
    }

    if ( min > max ) min = max;

    smin = min;
    smax = max;
    //printf( "min = %d, max = %d\n", min, max );

    mid = min;
    while ( min < max ) {
        mid = ( max - min ) / 2 + min;
        //mid = ( min + max ) / 2;
        if ( strcmp( child->getText().data(), childAt( mid )->getText().data() ) < 0 ) {
            max = mid - 1;
        } else {
            min = mid + 1;
        }
    }
    
    if ( childCount() ) {
        // KLUDGE!
        mid -= 5;
        if ( mid < smin ) mid = smin;
        if ( ((Node*)childAt( mid ))->getType() != child->getType() ) mid++;
        for ( ; ( mid <= smax ) && ( strcmp( child->getText().data(), childAt( mid )->getText().data() ) > 0 ); mid++ );
        //printf( "index = %d, text = '%s'\n", mid, child->getText().data() );
        insertChild( mid, child );
    } else {
        //printf( "index = %d, text = '%s'\n", 0, child->getText().data() );
        insertChild( 0, child );
    }
}

int Node::getType()
{
    return _type;
}

bool Node::isType( int type )
{
    return type == NodeType;
}

void Node::expanding( bool expand )
{
    expand = TRUE;
}

void Node::expanded()
{
}

void Node::collapsed()
{
}

void Node::selected()
{
    KDat::instance()->hideInfo();
}

void Node::popupMenu( const QPoint& )
{
}

TapeNode::TapeNode( Tape* tape )
        : Node( TapeNodeType, tape->getName(), *ImageCache::instance()->getTape() ),
          _tape( tape )
{
    setDelayedExpanding( TRUE );
}

Tape* TapeNode::getTape()
{
    return _tape;
}

bool TapeNode::validate()
{
    bool changed = _tape->getName() != getText();
    if ( changed ) {
        setText( _tape->getName() );
    }

    if ( ( childCount() == 0 ) && ( !isExpanded() ) ) {
        return changed;
    }
    
    // Add/update existing archives.
    QListIterator<Archive> i( _tape->getChildren() );
    ArchiveNode* n;
    for ( ; i.current(); ++i ) {
        n = findArchiveNode( i.current() );
        if ( n ) {
            if ( n->validate() ) {
                bool select = ( owner->getCurrentItem() == n );
                removeChild( n );
                insertNode( n );
                if ( select ) {
                    owner->setCurrentItem( owner->itemRow( n ) );
                }
            }
        } else {
            insertNode( new ArchiveNode( i.current() ) );
        }
    }

    // Remove deleted archives.
    Archive* a;
    for ( uint j = 0; j < childCount(); ) {
        a = ((ArchiveNode*)childAt( j ))->getArchive();
        for ( i.toFirst(); i.current(); ++i ) {
            if ( i.current() == a ) {
                break;
            }
        }
        if ( !i.current() ) {
            removeChild( childAt( j ) );
        } else {
            j++;
        }
    }

    return changed;
}

bool TapeNode::isType( int type )
{
    if ( type == TapeNodeType ) {
        return TRUE;
    } else {
        return Node::isType( type );
    }
}

void TapeNode::expanding( bool expand )
{
    expand = TRUE;
    
    if ( childCount() > 0 ) {
        // We already have the children added
        return;
    }

    QListIterator<Archive> f( _tape->getChildren() );
    for ( ; f.current(); ++f ) {
        insertNode( new ArchiveNode( f.current() ) );
    }
}

void TapeNode::selected()
{
    KDat::instance()->showTapeInfo( _tape );
}

void TapeNode::popupMenu( const QPoint& p )
{
    KDat::instance()->popupTapeMenu( p );
}

ArchiveNode* TapeNode::findArchiveNode( Archive* archive )
{
    ArchiveNode* n = 0;
    for ( uint i = 0; i < childCount(); i++ ) {
        if ( ((ArchiveNode*)childAt( i ))->getArchive() == archive ) {
            n = (ArchiveNode*)childAt( i );
            break;
        }
    }

    return n;
}

ArchiveNode::ArchiveNode( Archive* archive )
        : Node( ArchiveNodeType, archive->getName(), *ImageCache::instance()->getArchive() ),
          _archive( archive )
{
    setDelayedExpanding( TRUE );
}

Archive* ArchiveNode::getArchive()
{
    return _archive;
}

bool ArchiveNode::validate()
{
    bool changed = _archive->getName() != getText();
    if ( changed ) {
        setText( _archive->getName() );
    }

    return changed;
}

bool ArchiveNode::isType( int type )
{
    if ( type == ArchiveNodeType ) {
        return TRUE;
    } else {
        return Node::isType( type );
    }
}

void ArchiveNode::expanding( bool expand )
{
    expand = TRUE;

    if ( childCount() > 0 ) {
        // We already have the children added.
        return;
    }

    QListIterator<File> f( _archive->getChildren() );
    for ( ; f.current(); ++f ) {
        if ( f.current()->getName()[f.current()->getName().length()-1] == '/' ) {
            insertNode( new TapeDirectoryNode( f.current() ) );
        } else {
            insertNode( new TapeFileNode( f.current() ) );
        }
    }
}

void ArchiveNode::selected()
{
    KDat::instance()->showArchiveInfo( _archive );
}

void ArchiveNode::popupMenu( const QPoint& p )
{
    KDat::instance()->popupArchiveMenu( p );
}

TapeDirectoryNode::TapeDirectoryNode( File* file )
        : Node( TapeDirectoryNodeType, "", *ImageCache::instance()->getFolderClosed() ),
          _file( file )
{
    setDelayedExpanding( TRUE );

    int len  = _file->getName().length();
    int idx1 = _file->getName().findRev( '/', len - 2 );
    if ( idx1 < 0 ) {
        setText( _file->getName().left( len - 1 ) );
    } else {
        setText( _file->getName().mid( idx1 + 1, len - idx1 - 2 ) );
    }
}

File* TapeDirectoryNode::getFile()
{
    return _file;
}

bool TapeDirectoryNode::isType( int type )
{
    if ( type == TapeDirectoryNodeType ) {
        return TRUE;
    } else {
        return Node::isType( type );
    }
}

void TapeDirectoryNode::expanding( bool expand )
{
    expand = TRUE;

    if ( childCount() > 0 ) {
        // We already have the children added.
        return;
    }

    QListIterator<File> f( _file->getChildren() );
    for ( ; f.current(); ++f ) {
        if ( f.current()->getName()[f.current()->getName().length()-1] == '/' ) {
            insertNode( new TapeDirectoryNode( f.current() ) );
        } else {
            insertNode( new TapeFileNode( f.current() ) );
        }
    }
}

void TapeDirectoryNode::expanded()
{
    setPixmap( *ImageCache::instance()->getFolderOpen() );
}

void TapeDirectoryNode::collapsed()
{
    setPixmap( *ImageCache::instance()->getFolderClosed() );
}

void TapeDirectoryNode::selected()
{
    KDat::instance()->showTapeFileInfo( _file );
}

TapeFileNode::TapeFileNode( File* file )
        : Node( TapeFileNodeType, file->getName(), *ImageCache::instance()->getFile() ),
          _file( file )
{
    int len  = _file->getName().length();
    int idx1 = _file->getName().findRev( '/', len - 1 );
    if ( idx1 < 0 ) {
        setText( _file->getName().left( len ) );
    } else {
        setText( _file->getName().mid( idx1 + 1, len - idx1 - 1 ) );
    }
}

File* TapeFileNode::getFile()
{
    return _file;
}

bool TapeFileNode::isType( int type )
{
    if ( type == TapeFileNodeType ) {
        return TRUE;
    } else {
        return Node::isType( type );
    }
}

void TapeFileNode::selected()
{
    KDat::instance()->showTapeFileInfo( _file );
}

SelectableNode::SelectableNode( int type, const char* text, const QPixmap& pixmap, int state )
        : Node( type, text, pixmap ),
          _state( state )
{
}

void SelectableNode::doUpdateState()
{
    bool oneSelected = FALSE;
    bool hasSelected = FALSE;
    bool allSelected = TRUE;
    for ( uint i = 0; i < childCount(); i++ ) {
        switch ( ((SelectableNode*)childAt( i ))->_state ) {
            case SelAll:
                oneSelected = TRUE;
                hasSelected = TRUE;
                break;

            case SelSome:
                hasSelected = TRUE;
                allSelected = FALSE;
                break;
                
            case SelNone:
                allSelected = FALSE;
                break;
        }
    }

    if ( allSelected ) {
        _state = SelAll;
    } else if ( oneSelected || hasSelected ) {
        _state = SelSome;
    } else {
        _state = SelNone;
    }

    if ( ( getParent() ) && ( getParent()->getParent() ) && ( ((Node*)getParent())->isType( SelectableNodeType ) ) ) {
        ((SelectableNode*)getParent())->doUpdateState();
    }
}

void SelectableNode::doSetSelected( bool select )
{
    // All my children get the same selection state.
    for ( uint i = 0; i < childCount(); i++ ) {
        if ( select ) {
            if ( !((SelectableNode*)childAt( i ))->isSelected() ) {
                ((SelectableNode*)childAt( i ))->doSetSelected( TRUE );
            }
        } else {
            if ( ((SelectableNode*)childAt( i ))->isSelected() || ((SelectableNode*)childAt( i ))->hasSelectedChildren() ) {
                ((SelectableNode*)childAt( i ))->doSetSelected( FALSE );
            }
        }
    }

    if ( select ) {
        _state = SelAll;
    } else {
        _state = SelNone;
    }
}

const QPixmap* SelectableNode::getSelectPixmap() const
{
    switch ( _state ) {
        case SelAll:
            return ImageCache::instance()->getSelectAll();

        case SelNone:
            return ImageCache::instance()->getSelectNone();

        case SelSome:
            return ImageCache::instance()->getSelectSome();
            
        default:
            return 0;
    }
}

bool SelectableNode::mousePressEvent( const QPoint& point )
{
    if ( _selectRect.contains( point ) ) {
        switch ( _state ) {
            case SelAll:
                setSelected( FALSE );
                break;

            case SelNone:
                setSelected( TRUE );
                break;

            case SelSome:
                setSelected( TRUE );
                break;
        }
        return TRUE;
    } else {
        return FALSE;
    }
}

void SelectableNode::paint( QPainter* p, int indent, const QColorGroup& cg,
                            bool highlighted ) const
{
    assert(getParent() != 0);		/* can't paint root item */

    p->save();
    p->setPen(cg.text());
    p->setBackgroundColor(cg.base());

    int cellHeight = height(p->fontMetrics());

    if (doTree) {
	paintTree(p, indent, cellHeight);
    }

    /*
     * If this item has at least one child and expand button drawing is
     * enabled, set the rect for the expand button for later mouse press
     * bounds checking, and draw the button.
     */
    if (doExpandButton && (child || delayedExpanding)) {
	paintExpandButton(p, indent, cellHeight);
    }

    const QPixmap* pm = getSelectPixmap();
    p->drawPixmap( indent, ( height() - pm->height() ) / 2, *pm );
    _selectRect.setRect( indent, ( height() - pm->height() ) / 2, 12, 12 );

    // now draw the item pixmap and text, if applicable
    int pixmapX = indent + 4 + 12;
    int pixmapY = (cellHeight - pixmap.height()) / 2;
    p->drawPixmap(pixmapX, pixmapY, pixmap);

    if (doText) {
	paintText(p, indent, cellHeight, cg, highlighted);
    }
    p->restore();
}

void SelectableNode::paintText( QPainter* p, int indent, int cellHeight,
                                const QColorGroup& cg, bool highlighted ) const
{
    int textX = indent + 12 + 4 + pixmap.width() + 4;
    int textY = cellHeight - ((cellHeight - p->fontMetrics().ascent() - 
			       p->fontMetrics().leading()) / 2);
    if (highlighted) {
	paintHighlight(p, indent, cg,
		       owner->hasFocus(), owner->style());
	p->setPen(cg.base());
	p->setBackgroundColor(cg.text());
    }
    else {
	p->setPen(cg.text());
	p->setBackgroundColor(cg.base());
    }
    p->drawText(textX, textY, text);
}

int SelectableNode::width( int indent, const QFontMetrics& fm ) const
{
    int maxWidth = pixmap.width();
    maxWidth += (4 + 12 + 4 + fm.width(text));
    return maxWidth == 0  ?  -1  :  indent + maxWidth + 3;
}

QRect SelectableNode::textBoundingRect(int indent) const
{
    const QFontMetrics& fm = owner->fontMetrics();
    int cellHeight = height(fm);
    int rectX = indent + 12 + 4 + pixmap.width() + 3;
    int rectY = (cellHeight - fm.ascent() - fm.leading()) / 2 + 2;
    int rectW = fm.width(text) + 1;
    int rectH = fm.ascent() + fm.leading();
    return QRect(rectX, rectY, rectW, rectH);
}

bool SelectableNode::isType( int type )
{
    if ( type == SelectableNodeType ) {
        return TRUE;
    } else {
        return Node::isType( type );
    }
}

bool SelectableNode::isSelected()
{
    return _state == SelAll;
}

bool SelectableNode::hasSelectedChildren()
{
    return _state == SelAll || _state == SelSome;
}

void SelectableNode::setSelected( bool select )
{
    doSetSelected( select );

    if ( ( getParent() ) && ( getParent()->getParent() ) && ( ((Node*)getParent())->isType( SelectableNodeType ) ) ) {
        ((SelectableNode*)getParent())->doUpdateState();
    }

    owner->repaint();

    KDat::instance()->configureUI( TapeManager::instance()->getMountedTape() );
}

RangeableNode::RangeableNode( int type, const char* text, const QPixmap& pixmap, int state )
        : SelectableNode( type, text, pixmap, state )
{
}

bool RangeableNode::isType( int type )
{
    if ( type == RangeableNodeType ) {
        return TRUE;
    } else {
        return SelectableNode::isType( type );
    }
}

MountedArchiveNode::MountedArchiveNode( Archive* archive )
        : RangeableNode( MountedArchiveNodeType, archive->getName(), *ImageCache::instance()->getArchive(), SelNone ),
          _archive( archive )
{
    setDelayedExpanding( TRUE );
}

Archive* MountedArchiveNode::getArchive()
{
    return _archive;
}

const QList<Range>& MountedArchiveNode::getRanges()
{
    return _archive->getRanges();
}

bool MountedArchiveNode::validate()
{
    bool changed = _archive->getName() != getText();
    if ( changed ) {
        setText( _archive->getName() );
    }

    return changed;
}

void MountedArchiveNode::setSelected( bool select )
{
    if ( select ) {
        // Deselect all other archives.
        Node* parent = (Node*)getParent();
        for ( int i = parent->childCount() - 1; i >= 0; i-- ) {
            if ( parent->childAt( i ) != this ) {
                ((SelectableNode*)parent->childAt( i ))->setSelected( FALSE );
            }
        }
    }

    SelectableNode::setSelected( select );
}

bool MountedArchiveNode::isType( int type )
{
    if ( type == MountedArchiveNodeType ) {
        return TRUE;
    } else {
        return RangeableNode::isType( type );
    }
}

void MountedArchiveNode::expanding( bool expand )
{
    expand = TRUE;

    if ( childCount() > 0 ) {
        // We already have the children added.
        return;
    }

    QListIterator<File> f( _archive->getChildren() );
    for ( ; f.current(); ++f ) {
        if ( f.current()->getName()[f.current()->getName().length()-1] == '/' ) {
            insertNode( new MountedTapeDirectoryNode( f.current(), _state == SelSome ? SelNone : _state ) );
        } else {
            insertNode( new MountedTapeFileNode( f.current(), _state == SelSome ? SelNone : _state ) );
        }
    }
}

void MountedArchiveNode::selected()
{
    KDat::instance()->showArchiveInfo( _archive );
}

void MountedArchiveNode::popupMenu( const QPoint& p )
{
    KDat::instance()->popupMountedArchiveMenu( p );
}

MountedTapeDirectoryNode::MountedTapeDirectoryNode( File* file, int state )
        : RangeableNode( MountedTapeDirectoryNodeType, "", *ImageCache::instance()->getFolderClosed(), state ),
          _file( file )
{
    assert( _file > (File*)0x1 );

    setDelayedExpanding( TRUE );

    int len  = _file->getName().length();
    int idx1 = _file->getName().findRev( '/', len - 2 );
    if ( idx1 < 0 ) {
        setText( _file->getName().left( len - 1 ) );
    } else {
        setText( _file->getName().mid( idx1 + 1, len - idx1 - 2 ) );
    }
}

File* MountedTapeDirectoryNode::getFile()
{
    return _file;
}

const char* MountedTapeDirectoryNode::getFullPath()
{
    if ( _fullPath.length() == 0 ) {
        _fullPath = getText() + "/";
        for ( Node* parent = (Node*)getParent(); !parent->isType( MountedArchiveNodeType ); parent = (Node*)parent->getParent() ) {
            _fullPath.prepend( "/" );
            _fullPath.prepend( parent->getText() );
        }
    }

    return _fullPath;
}

const QList<Range>& MountedTapeDirectoryNode::getRanges()
{
    return _file->getRanges();
}

void MountedTapeDirectoryNode::setSelected( bool select )
{
    if ( select ) {
        // Deselect all other archives.
        Node* parent = (Node*)getParent();
        Node* arcNode = 0;
        while ( !parent->isType( TapeDriveNodeType ) ) {
            if ( parent->isType( MountedArchiveNodeType ) ) {
                arcNode = parent;
            }
            parent = (Node*)parent->getParent();
        }
        for ( int i = parent->childCount() - 1; i >= 0; i-- ) {
            if ( parent->childAt( i ) != arcNode ) {
                ((SelectableNode*)parent->childAt( i ))->setSelected( FALSE );
            }
        }
    }

    SelectableNode::setSelected( select );
}

bool MountedTapeDirectoryNode::isType( int type )
{
    if ( type == MountedTapeDirectoryNodeType ) {
        return TRUE;
    } else {
        return RangeableNode::isType( type );
    }
}

void MountedTapeDirectoryNode::expanding( bool expand )
{
    expand = TRUE;

    if ( childCount() > 0 ) {
        // We already have the children added.
        return;
    }

    QListIterator<File> f( _file->getChildren() );
    for ( ; f.current(); ++f ) {
        if ( f.current()->getName()[f.current()->getName().length()-1] == '/' ) {
            insertNode( new MountedTapeDirectoryNode( f.current(), _state == SelSome ? SelNone : _state ) );
        } else {
            insertNode( new MountedTapeFileNode( f.current(), _state == SelSome ? SelNone : _state ) );
        }
    }
}

void MountedTapeDirectoryNode::expanded()
{
    setPixmap( *ImageCache::instance()->getFolderOpen() );
}

void MountedTapeDirectoryNode::collapsed()
{
    setPixmap( *ImageCache::instance()->getFolderClosed() );
}

void MountedTapeDirectoryNode::selected()
{
    KDat::instance()->showTapeFileInfo( _file );
}

void MountedTapeDirectoryNode::popupMenu( const QPoint& p )
{
    KDat::instance()->popupMountedTapeFileMenu( p );
}

MountedTapeFileNode::MountedTapeFileNode( File* file, int state )
        : RangeableNode( MountedTapeFileNodeType, file->getName(), *ImageCache::instance()->getFile(), state ),
          _file( file )
{
    int len  = _file->getName().length();
    int idx1 = _file->getName().findRev( '/', len - 1 );
    if ( idx1 < 0 ) {
        setText( _file->getName().left( len ) );
    } else {
        setText( _file->getName().mid( idx1 + 1, len - idx1 - 1 ) );
    }
}

File* MountedTapeFileNode::getFile()
{
    return _file;
}

const char* MountedTapeFileNode::getFullPath()
{
    if ( _fullPath.length() == 0 ) {
        _fullPath = getText();
        _fullPath.detach();
        for ( Node* parent = (Node*)getParent(); !parent->isType( MountedArchiveNodeType ); parent = (Node*)parent->getParent() ) {
            _fullPath.prepend( "/" );
            _fullPath.prepend( parent->getText() );
        }
    }

    return _fullPath;
}

const QList<Range>& MountedTapeFileNode::getRanges()
{
    return _file->getRanges();
}

void MountedTapeFileNode::setSelected( bool select )
{
    if ( select ) {
        // Deselect all other archives.
        Node* parent = (Node*)getParent();
        Node* arcNode = 0;
        while ( !parent->isType( TapeDriveNodeType ) ) {
            if ( parent->isType( MountedArchiveNodeType ) ) {
                arcNode = parent;
            }
            parent = (Node*)parent->getParent();
        }
        for ( int i = parent->childCount() - 1; i >= 0; i-- ) {
            if ( parent->childAt( i ) != arcNode ) {
                ((SelectableNode*)parent->childAt( i ))->setSelected( FALSE );
            }
        }
    }

    SelectableNode::setSelected( select );
}

bool MountedTapeFileNode::isType( int type )
{
    if ( type == MountedTapeFileNodeType ) {
        return TRUE;
    } else {
        return RangeableNode::isType( type );
    }
}

void MountedTapeFileNode::selected()
{
    KDat::instance()->showTapeFileInfo( _file );
}

void MountedTapeFileNode::popupMenu( const QPoint& p )
{
    KDat::instance()->popupMountedTapeFileMenu( p );
}

ArchiveableNode::ArchiveableNode( int type, const char* text, const QPixmap& pixmap, int state )
        : SelectableNode( type, text, pixmap, state )
{
}

bool ArchiveableNode::isType( int type )
{
    if ( type == ArchiveableNodeType ) {
        return TRUE;
    } else {
        return SelectableNode::isType( type );
    }
}

RootNode::RootNode()
        : ArchiveableNode( RootNodeType, "/", *ImageCache::instance()->getFolderClosed(), SelNone ),
          _mtime( -1 )
{
    setDelayedExpanding( TRUE );
}

const char* RootNode::getFullPath()
{
    if ( _fullPath.length() == 0 ) {
        _fullPath = "/";
    }

    return _fullPath;
}

bool RootNode::isType( int type )
{
    if ( type == RootNodeType ) {
        return TRUE;
    } else {
        return ArchiveableNode::isType( type );
    }
}

void RootNode::expanding( bool expand )
{
    expand = TRUE;

    // If we already have some children, check to see if the directory has been modified.
    if ( childCount() > 0 ) {
        struct stat statinfo;
        if ( stat( "/", &statinfo ) < 0 ) {
            printf( "Can't stat '/'\n" );
            expand = FALSE;
            return;
        }

        if ( statinfo.st_mtime == _mtime ) {
            // Directory has not been modified.
            return;
        }

        _mtime = statinfo.st_mtime;

        // Remove all the children.
        Node* n;
        while ( ( n = (Node*)getChild() ) ) {
            removeChild( n );
            delete n;
        }
    } else {
        struct stat statinfo;
        if ( stat( "/", &statinfo ) < 0 ) {
            printf( "Can't stat '/'\n" );
            expand = FALSE;
            return;
        }

        _mtime = statinfo.st_mtime;
    }

    QDir dir( "/" );
    if ( dir.exists() ) {
        // Make sure we can read the directory.
        if ( !dir.isReadable() ) {
            //KMsgBox::message( this, i18n( "KDat: Error" ), i18n( "The directory cannot be read." ), KMsgBox::EXCLAMATION );
            return;
        }

        // Fill in the child's children.
        const QFileInfoList* list = dir.entryInfoList( QDir::Hidden | QDir::Files | QDir::Dirs, QDir::Name | QDir::DirsFirst );
        if ( list ) {
            QFileInfoListIterator it( *list );
            for ( ; it.current(); ++it ) {
                if ( ( it.current()->fileName() != "." ) && ( it.current()->fileName() != ".." ) ) {
                    if ( it.current()->isDir() ) {
                        appendChild( new DirectoryNode( it.current()->fileName(), _state == SelSome ? SelNone : _state ) );
                    } else {
                        appendChild( new FileNode( it.current()->fileName(), _state == SelSome ? SelNone : _state ) );
                    }
                }
            }
        }
    }
}

void RootNode::expanded()
{
    setPixmap( *ImageCache::instance()->getFolderOpen() );
}

void RootNode::collapsed()
{
    setPixmap( *ImageCache::instance()->getFolderClosed() );
}

void RootNode::selected()
{
    KDat::instance()->showFileInfo( "/" );
}

void RootNode::popupMenu( const QPoint& p )
{
    KDat::instance()->popupLocalFileMenu( p );
}

DirectoryNode::DirectoryNode( const char* text, int state )
        : ArchiveableNode( DirectoryNodeType, text, *ImageCache::instance()->getFolderClosed(), state ),
          _mtime( -1 )
{
    setDelayedExpanding( TRUE );
}

const char* DirectoryNode::getFullPath()
{
    if ( _fullPath.length() == 0 ) {
        _fullPath = ((ArchiveableNode*)getParent())->getFullPath() + getText() + "/";
    }

    return _fullPath;
}

bool DirectoryNode::isType( int type )
{
    if ( type == DirectoryNodeType ) {
        return TRUE;
    } else {
        return ArchiveableNode::isType( type );
    }
}

void DirectoryNode::expanding( bool expand )
{
    expand = TRUE;

    // Construct the full path.
    QString path;
    Node* n;
    for ( n = this; n->getType() != RootNodeType; n = (Node*)n->getParent() ) {
        path.prepend( "/" );
        path.prepend( n->getText() );
    }
    path.prepend( "/" );

    // If we already have some children, check to see if the directory has been modified.
    if ( childCount() > 0 ) {
        struct stat statinfo;
        if ( stat( path, &statinfo ) < 0 ) {
            printf( "Can't stat '%s'\n", path.data() );
            expand = FALSE;
            return;
        }

        if ( statinfo.st_mtime == _mtime ) {
            // Directory has not been modified.
            return;
        }

        _mtime = statinfo.st_mtime;

        // Remove all the children.
        Node* n;
        while ( ( n = (Node*)getChild() ) ) {
            removeChild( n );
            delete n;
        }
    } else {
        struct stat statinfo;
        if ( stat( path, &statinfo ) < 0 ) {
            printf( "Can't stat '%s'\n", path.data() );
            expand = FALSE;
            return;
        }

        _mtime = statinfo.st_mtime;
    }

    QDir dir( path );
    if ( dir.exists() ) {
        // Make sure we can read the directory.
        if ( !dir.isReadable() ) {
            //KMsgBox::message( this, i18n( "KDat: Error" ), i18n( "The directory cannot be read." ), KMsgBox::EXCLAMATION );
            return;
        }

        // Fill in the child's children.
        const QFileInfoList* list = dir.entryInfoList( QDir::Hidden | QDir::Files | QDir::Dirs, QDir::Name | QDir::DirsFirst );
        if ( list ) {
            QFileInfoListIterator it( *list );
            for ( ; it.current(); ++it ) {
                if ( ( it.current()->fileName() != "." ) && ( it.current()->fileName() != ".." ) ) {
                    if ( it.current()->isDir() ) {
                        appendChild( new DirectoryNode( it.current()->fileName(), _state == SelSome ? SelNone : _state ) );
                    } else {
                        appendChild( new FileNode( it.current()->fileName(), _state == SelSome ? SelNone : _state ) );
                    }
                }
            }
        }
    }
}

void DirectoryNode::expanded()
{
    setPixmap( *ImageCache::instance()->getFolderOpen() );
}

void DirectoryNode::collapsed()
{
    setPixmap( *ImageCache::instance()->getFolderClosed() );
}

void DirectoryNode::selected()
{
    // Construct the full path.
    QString path;
    Node* n;
    for ( n = this; n->getType() != RootNodeType; n = (Node*)n->getParent() ) {
        path.prepend( "/" );
        path.prepend( n->getText() );
    }
    path.prepend( "/" );
    path = path.left( path.length() - 1 );

    KDat::instance()->showFileInfo( path );
}

void DirectoryNode::popupMenu( const QPoint& p )
{
    KDat::instance()->popupLocalFileMenu( p );
}

FileNode::FileNode( const char* text, int state )
        : ArchiveableNode( FileNodeType, text, *ImageCache::instance()->getFile(), state )
{
}

const char* FileNode::getFullPath()
{
    if ( _fullPath.length() == 0 ) {
        _fullPath = ((ArchiveableNode*)getParent())->getFullPath() + getText();
    }

    return _fullPath;
}

bool FileNode::isType( int type )
{
    if ( type == FileNodeType ) {
        return TRUE;
    } else {
        return ArchiveableNode::isType( type );
    }
}

void FileNode::selected()
{
    // Construct the full path.
    QString path;
    Node* n;
    for ( n = this; n->getType() != RootNodeType; n = (Node*)n->getParent() ) {
        path.prepend( "/" );
        path.prepend( n->getText() );
    }
    path.prepend( "/" );
    path = path.left( path.length() - 1 );

    KDat::instance()->showFileInfo( path );
}

void FileNode::popupMenu( const QPoint& p )
{
    KDat::instance()->popupLocalFileMenu( p );
}

TapeDriveNode::TapeDriveNode()
        : Node( TapeDriveNodeType, i18n( "<no tape>" ), *ImageCache::instance()->getTapeUnmounted() )
{
    setDelayedExpanding( TRUE );

    connect( TapeManager::instance(), SIGNAL( sigTapeMounted() )        , this, SLOT( slotTapeMounted() ) );
    connect( TapeManager::instance(), SIGNAL( sigTapeUnmounted() )      , this, SLOT( slotTapeUnmounted() ) );
    connect( TapeManager::instance(), SIGNAL( sigTapeModified( Tape* ) ), this, SLOT( slotTapeModified( Tape* ) ) );
}

bool TapeDriveNode::isType( int type )
{
    if ( type == TapeDriveNodeType ) {
        return TRUE;
    } else {
        return Node::isType( type );
    }
}

void TapeDriveNode::expanding( bool expand )
{
    if ( !TapeManager::instance()->getMountedTape() ) {
        expand = FALSE;
        return;
    }

    expand = TRUE;
    
    if ( childCount() > 0 ) {
        // We already have the children added
        return;
    }

    QListIterator<Archive> f( TapeManager::instance()->getMountedTape()->getChildren() );
    for ( ; f.current(); ++f ) {
        insertNode( new MountedArchiveNode( f.current() ) );
    }
}

void TapeDriveNode::selected()
{
    if ( TapeManager::instance()->getMountedTape() ) {
        KDat::instance()->showTapeInfo( TapeManager::instance()->getMountedTape() );
    } else {
        KDat::instance()->hideInfo();
    }
}

void TapeDriveNode::popupMenu( const QPoint& p )
{
    KDat::instance()->popupTapeDriveMenu( p );
}

void TapeDriveNode::slotTapeMounted()
{
    setPixmap( *ImageCache::instance()->getTapeMounted() );
    setText( TapeManager::instance()->getMountedTape()->getName() );

    if ( this == owner->getCurrentItem() ) {
        KDat::instance()->showTapeInfo( TapeManager::instance()->getMountedTape() );
    }

    if ( isExpanded() ) {
        bool dummy = TRUE;
        expanding( dummy );
    }
}

void TapeDriveNode::slotTapeUnmounted()
{
    setPixmap( *ImageCache::instance()->getTapeUnmounted() );
    setText( i18n( "<no tape>" ) );

    // Remove all the children.
    Node* n;
    while ( ( n = (Node*)getChild() ) ) {
        Node::removeChild( n );
        delete n;
    }

    if ( this == owner->getCurrentItem() ) {
        KDat::instance()->hideInfo();
    }
}

void TapeDriveNode::slotTapeModified( Tape* tape )
{
    if ( TapeManager::instance()->getMountedTape() != tape ) {
        return;
    }

    setText( tape->getName() );

    if ( owner->getCurrentItem() == this ) {
        KDat::instance()->showTapeInfo( TapeManager::instance()->getMountedTape() );
    }

    if ( ( childCount() == 0 ) && ( !isExpanded() ) ) {
        return;
    }
    
    // Add/update existing archives.
    QListIterator<Archive> i( tape->getChildren() );
    MountedArchiveNode* n;
    for ( ; i.current(); ++i ) {
        n = findArchiveNode( i.current() );
        if ( n ) {
            if ( n->validate() ) {
                bool select = ( owner->getCurrentItem() == n );
                Node::removeChild( n );
                insertNode( n );
                if ( select ) {
                    owner->setCurrentItem( owner->itemRow( n ) );
                }
            }
        } else {
            insertNode( new MountedArchiveNode( i.current() ) );
        }
    }

    // Remove deleted archives.
    Archive* a;
    for ( uint j = 0; j < childCount(); ) {
        a = ((MountedArchiveNode*)childAt( j ))->getArchive();
        for ( i.toFirst(); i.current(); ++i ) {
            if ( i.current() == a ) {
                break;
            }
        }
        if ( !i.current() ) {
            Node::removeChild( childAt( j ) );
        } else {
            j++;
        }
    }
}

MountedArchiveNode* TapeDriveNode::findArchiveNode( Archive* archive )
{
    MountedArchiveNode* n = 0;
    for ( uint i = 0; i < childCount(); i++ ) {
        if ( ((MountedArchiveNode*)childAt( i ))->getArchive() == archive ) {
            n = (MountedArchiveNode*)childAt( i );
            break;
        }
    }

    return n;
}

TapeIndexRootNode::TapeIndexRootNode()
        : Node( TapeIndexRootNodeType, i18n( "Tape Indexes" ), *ImageCache::instance()->getFolderClosed() )
{
    setDelayedExpanding( TRUE );

    connect( TapeManager::instance(), SIGNAL( sigTapeAdded( Tape* ) )   , this, SLOT( slotTapeAdded( Tape* ) ) );
    connect( TapeManager::instance(), SIGNAL( sigTapeRemoved( Tape* ) ) , this, SLOT( slotTapeRemoved( Tape* ) ) );
    connect( TapeManager::instance(), SIGNAL( sigTapeModified( Tape* ) ), this, SLOT( slotTapeModified( Tape* ) ) );
}

bool TapeIndexRootNode::isType( int type )
{
    if ( type == TapeIndexRootNodeType ) {
        return TRUE;
    } else {
        return Node::isType( type );
    }
}

void TapeIndexRootNode::expanding( bool expand )
{
    expand = TRUE;

    if ( childCount() > 0 ) {
        return;
    }

    QStrListIterator i( TapeManager::instance()->getTapeIDs() );
    for ( ; i.current(); ++i ) {
        Tape* tape = TapeManager::instance()->findTape( i.current() );
        if ( tape ) {
            insertNode( new TapeNode( tape ) );
        }
    }
}

void TapeIndexRootNode::expanded()
{
    setPixmap( *ImageCache::instance()->getFolderOpen() );
}

void TapeIndexRootNode::collapsed()
{
    setPixmap( *ImageCache::instance()->getFolderClosed() );
}

void TapeIndexRootNode::slotTapeAdded( Tape* tape )
{
    if ( ( childCount() > 0 ) || ( isExpanded() ) ) {
        insertNode( new TapeNode( tape ) );
    }
}

void TapeIndexRootNode::slotTapeRemoved( Tape* tape )
{
    if ( ( childCount() > 0 ) || ( isExpanded() ) ) {
        Node* n = findTapeNode( tape );
        if ( n ) {
            Node::removeChild( n );
            delete n;
        }
    }
}

void TapeIndexRootNode::slotTapeModified( Tape* tape )
{
    TapeNode* n = findTapeNode( tape );
    if ( !n ) {
        return;
    }

    if ( n->validate() ) {
        bool select = ( owner->getCurrentItem() == n );
        Node::removeChild( n );
        insertNode( n );
        if ( select ) {
            owner->setCurrentItem( owner->itemRow( n ) );
        }
    }
}

TapeNode* TapeIndexRootNode::findTapeNode( Tape* tape )
{
    TapeNode* n = 0;
    for ( uint i = 0; i < childCount(); i++ ) {
        n = (TapeNode*)childAt( i );
        if ( n->getTape() == tape ) {
            return n;
        }
    }

    return 0;
}

BackupProfileNode::BackupProfileNode( BackupProfile* backupProfile )
        : Node( BackupProfileNodeType, backupProfile->getName(), *ImageCache::instance()->getFile() ),
          _backupProfile( backupProfile )
{
}

BackupProfile* BackupProfileNode::getBackupProfile()
{
    return _backupProfile;
}

bool BackupProfileNode::validate()
{
    bool changed = _backupProfile->getName() != getText();
    if ( changed ) {
        setText( _backupProfile->getName() );
    }

    return changed;
}

bool BackupProfileNode::isType( int type )
{
    if ( type == BackupProfileNodeType ) {
        return TRUE;
    } else {
        return Node::isType( type );
    }
}

void BackupProfileNode::selected()
{
    KDat::instance()->showBackupProfileInfo( _backupProfile );
}

void BackupProfileNode::popupMenu( const QPoint& p )
{
    KDat::instance()->popupBackupProfileMenu( p );
}

BackupProfileRootNode::BackupProfileRootNode()
        : Node( BackupProfileRootNodeType, i18n( "Backup Profiles" ), *ImageCache::instance()->getFolderClosed() )
{
    setDelayedExpanding( TRUE );

    connect( BackupProfileManager::instance(), SIGNAL( sigBackupProfileAdded( BackupProfile* ) )   ,
             this, SLOT( slotBackupProfileAdded( BackupProfile* ) ) );
    connect( BackupProfileManager::instance(), SIGNAL( sigBackupProfileRemoved( BackupProfile* ) ) ,
             this, SLOT( slotBackupProfileRemoved( BackupProfile* ) ) );
    connect( BackupProfileManager::instance(), SIGNAL( sigBackupProfileModified( BackupProfile* ) ),
             this, SLOT( slotBackupProfileModified( BackupProfile* ) ) );
}

void BackupProfileRootNode::setSelected( BackupProfile* pBackupProfile )
{
    setExpanded( TRUE );
    bool dummy = TRUE;
    expanding( dummy );
    expanded();

    BackupProfileNode* pNode = findBackupProfileNode( pBackupProfile );
    if ( pNode ) {
        owner->setCurrentItem( owner->itemRow( pNode ) );
        pNode->selected();
    }

    KDat::instance()->configureUI( TapeManager::instance()->getMountedTape() );
}

bool BackupProfileRootNode::isType( int type )
{
    if ( type == BackupProfileRootNodeType ) {
        return TRUE;
    } else {
        return Node::isType( type );
    }
}

void BackupProfileRootNode::expanding( bool expand )
{
    expand = TRUE;

    if ( childCount() > 0 ) {
        return;
    }

    QStrListIterator i( BackupProfileManager::instance()->getBackupProfileNames() );
    for ( ; i.current(); ++i ) {
        BackupProfile* backupProfile = BackupProfileManager::instance()->findBackupProfile( i.current() );
        if ( backupProfile ) {
            insertNode( new BackupProfileNode( backupProfile ) );
        }
    }
}

void BackupProfileRootNode::expanded()
{
    setPixmap( *ImageCache::instance()->getFolderOpen() );
}

void BackupProfileRootNode::collapsed()
{
    setPixmap( *ImageCache::instance()->getFolderClosed() );
}

void BackupProfileRootNode::popupMenu( const QPoint& p )
{
    KDat::instance()->popupBackupProfileRootMenu( p );
}

void BackupProfileRootNode::slotBackupProfileAdded( BackupProfile* backupProfile )
{
    if ( ( childCount() > 0 ) || ( isExpanded() ) ) {
        insertNode( new BackupProfileNode( backupProfile ) );
    }
}

void BackupProfileRootNode::slotBackupProfileRemoved( BackupProfile* backupProfile )
{
    if ( ( childCount() > 0 ) || ( isExpanded() ) ) {
        Node* n = findBackupProfileNode( backupProfile );
        assert( n );
        Node::removeChild( n );
        delete n;
    }
}

void BackupProfileRootNode::slotBackupProfileModified( BackupProfile* backupProfile )
{
    BackupProfileNode* n = findBackupProfileNode( backupProfile );
    if ( !n ) {
        return;
    }

    if ( n->validate() ) {
        bool select = ( owner->getCurrentItem() == n );
        Node::removeChild( n );
        insertNode( n );
        if ( select ) {
            owner->setCurrentItem( owner->itemRow( n ) );
        }
    }
}

BackupProfileNode* BackupProfileRootNode::findBackupProfileNode( BackupProfile* backupProfile )
{
    BackupProfileNode* n = 0;
    for ( uint i = 0; i < childCount(); i++ ) {
        n = (BackupProfileNode*)childAt( i );
        if ( n->getBackupProfile() == backupProfile ) {
            return n;
        }
    }

    return 0;
}
