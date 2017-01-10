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

#ifndef _Node_h_
#define _Node_h_

#include <qobject.h>

#include "ktreeview.h"

class Archive;
class BackupProfile;
class File;
class RangeList;
class Tape;

/**
 * @short The base class for all tree nodes in KDat.
 */
class Node : public KTreeViewItem {
    int _type;
protected:
    void insertNode( Node* child );
public:
    enum {
        ArchiveableNodeType,
        ArchiveNodeType,
        BackupProfileNodeType,
        BackupProfileRootNodeType,
        DirectoryNodeType,
        FileNodeType,
        MountedArchiveNodeType,
        MountedTapeDirectoryNodeType,
        MountedTapeFileNodeType,
        NodeType,
        RangeableNodeType,
        RootNodeType,
        SelectableNodeType,
        TapeNodeType,
        TapeDirectoryNodeType,
        TapeDriveNodeType,
        TapeFileNodeType,
        TapeIndexRootNodeType
    };

    /**
     * Create a new tree node.
     *
     * @param type   Should be one of the enums.
     * @param text   Text label for the node.
     * @param pixmap A pixmap to display to the left of the text.
     */
    Node( int type, const char* text, const QPixmap& pixmap );

    /**
     * There must be a virtual destructor in the base class so that all the
     * destructors in the inherited classes get called.
     */
    virtual ~Node();

    /**
     * Get the node's type.
     *
     * @return The type of this node.  This information can be used to safely
     *         downcast a Node pointer.
     */
    int getType();

    /**
     * Determine whether the node is an instance of the given node type.
     *
     * @param type The type to compare against.
     */
    virtual bool isType( int type );

    /**
     * This method is called immediately before the node is to be expanded.
     * This method can be used to fill in the children of this node, or
     * prevent the node from expanding.  The default implementation does
     * nothing.
     *
     * @param expand The method sets this to TRUE to allow the node to be
     *               expanded or FALSE to prevent the node from being expanded.
     */
    virtual void expanding( bool expand=TRUE );

    /**
     * This method is called immediately after the node has been expanded.
     * The default implementation does nothing.
     */
    virtual void expanded();

    /**
     * This method is called immediately after the node has been collapsed.
     * The default implementation does nothing.
     */
    virtual void collapsed();

    /**
     * This method is called immediately after the node has been selected.
     * The default implementation does nothing.
     */
    virtual void selected();

    /**
     * This method is called when the user right-clicks the mouse over the
     * node.
     */
    virtual void popupMenu( const QPoint& );
};

class ArchiveNode;

/**
 * @short This node represents the root of a tape index.
 */
class TapeNode : public Node {
    Tape* _tape;

    ArchiveNode* findArchiveNode( Archive* archive );
public:
    /**
     * Create a tape node.
     *
     * @param tape A pointer to the tape index that this node represents.
     */
    TapeNode( Tape* tape );

    /**
     * Get the tape index associated with this node.
     *
     * @return A pointer to the tape index.
     */
    Tape* getTape();

    /**
     * Make sure that the displayed information matches the tape index.  This
     * method recurses through child nodes.
     *
     * @return TRUE if the node's text has changed.
     */
    bool validate();

    /**
     * Determine whether the node is an instance of the given node type.
     *
     * @param type The type to compare against.
     */
    virtual bool isType( int type );

    /**
     * Create child nodes for each archive in the tape index.
     *
     * @param expand This will always be set to TRUE.
     */
    virtual void expanding( bool expand=TRUE );

    /**
     * This method is called immediately after the node has been selected.
     */
    virtual void selected();

    /**
     * This method is called when the user right-clicks the mouse over the
     * node.
     */
    virtual void popupMenu( const QPoint& p );
};

/**
 * @short This node represents a single archive within a tape.
 */
class ArchiveNode : public Node {
    Archive* _archive;
public:
    /**
     * Create an archive node.
     *
     * @param archive A pointer to the archive that this node represents.
     */
    ArchiveNode( Archive* archive );

    /**
     * Get the archive associated with this node.
     *
     * @return A pointer to the archive.
     */
    Archive* getArchive();

    /**
     * Make sure that the displayed information matches the tape index.  This
     * method recurses through child nodes.
     *
     * @return TRUE if the node's text has changed.
     */
    bool validate();

    /**
     * Determine whether the node is an instance of the given node type.
     *
     * @param type The type to compare against.
     */
    virtual bool isType( int type );

    /**
     * Create child nodes for each top-level file/directory contained in the
     * archive.
     *
     * @param expand This will always be set to TRUE.
     */
    virtual void expanding( bool expand=TRUE );

    /**
     * This method is called immediately after the node has been selected.
     */
    virtual void selected();

    /**
     * This method is called when the user right-clicks the mouse over the
     * node.
     */
    virtual void popupMenu( const QPoint& p );
};

/**
 * @short This node represents a single directory within an archive.
 */
class TapeDirectoryNode : public Node {
    File* _file;
public:
    /**
     * Create a tape directory node.
     *
     * @param file  A pointer to the file that this node represents.
     */
    TapeDirectoryNode( File* file );

    /**
     * Get the file associated with this node.
     *
     * @return A pointer to the file.
     */
    File* getFile();

    /**
     * Determine whether the node is an instance of the given node type.
     *
     * @param type The type to compare against.
     */
    virtual bool isType( int type );
    
    /**
     * Create child nodes for each file contained in this directory.
     *
     * @param expand This will always be set to TRUE.
     */
    virtual void expanding( bool expand=TRUE );

    /**
     * Change the node's pixmap to an open folder.
     */
    virtual void expanded();

    /**
     * Change the node's pixmap to a closed folder.
     */
    virtual void collapsed();

    /**
     * This method is called immediately after the node has been selected.
     */
    virtual void selected();
};

/**
 * @short This node represents a single file within an archive.
 */
class TapeFileNode : public Node {
    File* _file;
public:
    /**
     * Create a tape file node.
     *
     * @param file  A pointer to the file that this node represents.
     */
    TapeFileNode( File* file );

    /**
     * Get the file associated with this node.
     *
     * @return A pointer to the file.
     */
    File* getFile();

    /**
     * Determine whether the node is an instance of the given node type.
     *
     * @param type The type to compare against.
     */
    virtual bool isType( int type );

    /**
     * This method is called immediately after the node has been selected.
     */
    virtual void selected();
};

/**
 * @short This class defines an interface for managing multiple selection of tree nodes.
 */
class SelectableNode : public Node {
protected:
    mutable QRect _selectRect;
    enum { SelAll, SelNone, SelSome };
    int _state;

    void doUpdateState();
    void doSetSelected( bool select );
    const QPixmap* getSelectPixmap() const;

    virtual bool mousePressEvent( const QPoint& point );
    virtual void paint( QPainter* p, int indent,
                        const QColorGroup& cg, bool highlighted ) const;
    virtual void paintText( QPainter* p, int indent, int cellHeight,
                            const QColorGroup& cg, bool highlighted ) const;
    virtual int width( int indent, const QFontMetrics& fm ) const;
    virtual QRect textBoundingRect( int indent ) const;
public:
    /**
     * Create a selectable node.
     *
     * @param type   Should be one of the enums.
     * @param text   Text label for the node.
     * @param pixmap A pixmap to display to the left of the text.
     * @param state  The initial selection state of the node.
     */
    SelectableNode( int type, const char* text, const QPixmap& pixmap, int state );

    /**
     * Determine whether the node is an instance of the given node type.
     *
     * @param type The type to compare against.
     */
    virtual bool isType( int type );

    /**
     * Determine whether the node and all of its children are selected.
     *
     * @return TRUE if the node is selected, otherwise FALSE.
     */
    bool isSelected();

    /**
     * Determine whether the node has at least one selected descendent.
     *
     * @return TRUE if at least one descendent is selected.
     */
    bool hasSelectedChildren();

    /**
     * Select/deselect this node and all of its children.
     *
     * @param selected TRUE means the node will be selected, FALSE means the
     *                 node will be deselected.
     */
    virtual void setSelected( bool selected );
};

/**
 * @short This class defines an interface for retrieving a list of selected tape blocks.
 */
class RangeableNode : public SelectableNode {
public:
    /**
     * Create a new rangeable node.
     *
     * @param type   Should be one of the enums.
     * @param text   Text label for the node.
     * @param pixmap A pixmap to display to the left of the text.
     * @param state  The initial selection state of the node.
     */
    RangeableNode( int type, const char* text, const QPixmap& pixmap, int state );

    /**
     * Get a list of all the selected ranges for the subtree rooted at this
     * node.
     */
    virtual const QList<Range>& getRanges() = 0;

    /**
     * Determine whether the node is an instance of the given node type.
     *
     * @param type The type to compare against.
     */
    virtual bool isType( int type );
};

/**
 * @short This node represents a single archive within a mounted tape.
 */
class MountedArchiveNode : public RangeableNode {
    Archive* _archive;
public:
    /**
     * Create an mounted archive node.
     *
     * @param archive A pointer to the archive that this node represents.
     */
    MountedArchiveNode( Archive* archive );

    /**
     * Get the archive associated with this node.
     *
     * @return A pointer to the archive.
     */
    Archive* getArchive();

    /**
     * Make sure that the displayed information matches the tape index.  This
     * method recurses through child nodes.
     *
     * @return TRUE if the node's text has changed.
     */
    bool validate();

    /**
     * Get a list of all the selected ranges for the subtree rooted at this
     * node.
     */
    virtual const QList<Range>& getRanges();

    /**
     * Select/deselect this node and all of its children.
     *
     * @param selected TRUE means the node will be selected, FALSE means the
     *                 node will be deselected.
     */
    virtual void setSelected( bool selected );

    /**
     * Determine whether the node is an instance of the given node type.
     *
     * @param type The type to compare against.
     */
    virtual bool isType( int type );

    /**
     * Create child nodes for each top-level file/directory contained in the
     * archive.
     *
     * @param expand This will always be set to TRUE.
     */
    virtual void expanding( bool expand=TRUE );

    /**
     * This method is called immediately after the node has been selected.
     */
    virtual void selected();

    /**
     * This method is called when the user right-clicks the mouse over the
     * node.
     */
    virtual void popupMenu( const QPoint& p );
};

/**
 * @short This node represents a single directory within a mounted archive.
 */
class MountedTapeDirectoryNode : public RangeableNode {
    File*   _file;
    QString _fullPath;
public:
    /**
     * Create a tape directory node.
     *
     * @param file  A pointer to the file that this node represents.
     * @param state The initial selection state of the node.
     */
    MountedTapeDirectoryNode( File* file, int state );

    /**
     * Get the file associated with this node.
     *
     * @return A pointer to the file.
     */
    File* getFile();

    /**
     * Return the "fully" qualified path name of this node.  It will probably
     * NOT begin with a '/'.
     *
     * @return The full path name of the directory.
     */
    const char* getFullPath();

    /**
     * Get a list of all the selected ranges for the subtree rooted at this
     * node.
     */
    virtual const QList<Range>& getRanges();

    /**
     * Select/deselect this node and all of its children.
     *
     * @param selected TRUE means the node will be selected, FALSE means the
     *                 node will be deselected.
     */
    virtual void setSelected( bool selected );

    /**
     * Determine whether the node is an instance of the given node type.
     *
     * @param type The type to compare against.
     */
    virtual bool isType( int type );
    
    /**
     * Create child nodes for each file contained in this directory.
     *
     * @param expand This will always be set to TRUE.
     */
    virtual void expanding( bool expand=TRUE );

    /**
     * Change the node's pixmap to an open folder.
     */
    virtual void expanded();

    /**
     * Change the node's pixmap to a closed folder.
     */
    virtual void collapsed();

    /**
     * This method is called immediately after the node has been selected.
     */
    virtual void selected();

    /**
     * This method is called when the user right-clicks the mouse over the
     * node.
     */
    virtual void popupMenu( const QPoint& p );
};

/**
 * @short This node represents a single file within a mounted archive.
 */
class MountedTapeFileNode : public RangeableNode {
    File*   _file;
    QString _fullPath;
public:
    /**
     * Create a tape file node.
     *
     * @param file  A pointer to the file that this node represents.
     * @param state The initial selection state of the node.
     */
    MountedTapeFileNode( File* file, int state );

    /**
     * Get the file associated with this node.
     *
     * @return A pointer to the file.
     */
    File* getFile();

    /**
     * Return the "fully" qualified path name of this node.  It will probably
     * NOT begin with a '/'.
     *
     * @return The full path name of the file.
     */
    const char* getFullPath();

    /**
     * Get a list of all the selected ranges for the subtree rooted at this
     * node.
     */
    virtual const QList<Range>& getRanges();

    /**
     * Select/deselect this node and all of its children.
     *
     * @param selected TRUE means the node will be selected, FALSE means the
     *                 node will be deselected.
     */
    virtual void setSelected( bool selected );

    /**
     * Determine whether the node is an instance of the given node type.
     *
     * @param type The type to compare against.
     */
    virtual bool isType( int type );

    /**
     * This method is called immediately after the node has been selected.
     */
    virtual void selected();

    /**
     * This method is called when the user right-clicks the mouse over the
     * node.
     */
    virtual void popupMenu( const QPoint& p );
};

/**
 * @short This defines an interface for archiveable nodes.
 */
class ArchiveableNode : public SelectableNode {
protected:
    QString _fullPath;
public:
    /**
     * Create a new archiveable node.
     *
     * @param type   Should be one of the enums.
     * @param text   Text label for the node.
     * @param pixmap A pixmap to display to the left of the text.
     * @param state  The initial selection state of the node.
     */
    ArchiveableNode( int type, const char* text, const QPixmap& pixmap, int state );

    /**
     * Compute the full path name of this node.  Nodes representing
     * directories end with a '/'.
     *
     * @return The full path of the file that this node represents.
     */
    virtual const char* getFullPath() = 0;

    /**
     * Determine whether the node is an instance of the given node type.
     *
     * @param type The type to compare against.
     */
    virtual bool isType( int type );
};

/**
 * @short This node represents the root directory of the local filesystem.
 */
class RootNode : public ArchiveableNode {
    int _mtime;
public:
    /**
     * Create a root directory node.
     */
    RootNode();

    /**
     * Compute the full path name of this node.  Nodes representing
     * directories end with a '/'.
     *
     * @return The full path of the file that this node represents.
     */
    virtual const char* getFullPath();

    /**
     * Determine whether the node is an instance of the given node type.
     *
     * @param type The type to compare against.
     */
    virtual bool isType( int type );

    /**
     * Create child nodes for each file contained in this directory.  If the
     * directory has been modified then all of the child nodes are recreated.
     *
     * @param expand This will always be set to TRUE.
     */
    virtual void expanding( bool expand=TRUE );

    /**
     * Change the node's pixmap to an open folder.
     */
    virtual void expanded();

    /**
     * Change the node's pixmap to a closed folder.
     */
    virtual void collapsed();

    /**
     * This method is called immediately after the node has been selected.
     */
    virtual void selected();

    /**
     * This method is called when the user right-clicks the mouse over the
     * node.
     */
    virtual void popupMenu( const QPoint& p );
};

/**
 * @short This node represents a directory on the local filesystem.
 */
class DirectoryNode : public ArchiveableNode {
    int _mtime;
public:
    /**
     * Create a directory node.
     *
     * @param text  The name of the directory.  The full path is determined by
     *              traversing the tree to the root node.
     * @param state The initial selection state of the node.
     */
    DirectoryNode( const char* text, int state );

    /**
     * Compute the full path name of this node.  Nodes representing
     * directories end with a '/'.
     *
     * @return The full path of the file that this node represents.
     */
    virtual const char* getFullPath();

    /**
     * Determine whether the node is an instance of the given node type.
     *
     * @param type The type to compare against.
     */
    virtual bool isType( int type );

    /**
     * Create child nodes for each file contained in this directory.  If the
     * directory has been modified then all of the child nodes are recreated.
     *
     * @param expand This will always be set to TRUE.
     */
    virtual void expanding( bool expand=TRUE );

    /**
     * Change the node's pixmap to an open folder.
     */
    virtual void expanded();

    /**
     * Change the node's pixmap to a closed folder.
     */
    virtual void collapsed();

    /**
     * This method is called immediately after the node has been selected.
     */
    virtual void selected();

    /**
     * This method is called when the user right-clicks the mouse over the
     * node.
     */
    virtual void popupMenu( const QPoint& p );
};

/**
 * @short This node represents a file on the local filesystem.
 */
class FileNode : public ArchiveableNode {
public:
    /**
     * Create a file node.
     *
     * @param text  The name of the file.  The full path is determined by
     *              traversing the tree to the root node.
     * @param state The initial selection state of the node.
     */
    FileNode( const char* text, int state );

    /**
     * Compute the full path name of this node.  Nodes representing
     * directories end with a '/'.
     *
     * @return The full path of the file that this node represents.
     */
    virtual const char* getFullPath();

    /**
     * Determine whether the node is an instance of the given node type.
     *
     * @param type The type to compare against.
     */
    virtual bool isType( int type );

    /**
     * This method is called immediately after the node has been selected.
     */
    virtual void selected();

    /**
     * This method is called when the user right-clicks the mouse over the
     * node.
     */
    virtual void popupMenu( const QPoint& p );
};

/**
 * @short This node represents a tape drive.
 */
class TapeDriveNode : public QObject, public Node {
    Q_OBJECT

    MountedArchiveNode* findArchiveNode( Archive* archive );
public:
    /**
     * Create a tape drive node.
     */
    TapeDriveNode();

    /**
     * Determine whether the node is an instance of the given node type.
     *
     * @param type The type to compare against.
     */
    virtual bool isType( int type );

    /**
     * Create child nodes for each archive in the tape index.
     *
     * @param expand This will always be set to TRUE.
     */
    virtual void expanding( bool expand=TRUE );

    /**
     * This method is called immediately after the node has been selected.
     */
    virtual void selected();

    /**
     * This method is called when the user right-clicks the mouse over the
     * node.
     */
    virtual void popupMenu( const QPoint& p );
public slots:
    /**
     * This slot is called when the tape is mounted.
     */
    void slotTapeMounted();

    /**
     * This slot is called when the tape is unmounted.
     */
    void slotTapeUnmounted();

    /**
     * Locate the child associated with the modified tape index, and make sure
     * that the displayed information (including all children) is updated.
     *
     * @param tape A pointer to the tape index that was modified.
     */
    void slotTapeModified( Tape* tape );
};

/**
 * @short This node represents the root of the tape index subtree.
 */
class TapeIndexRootNode : public QObject, public Node {
    Q_OBJECT

    TapeNode* findTapeNode( Tape* tape );
public:
    /**
     * Create a tape index root.
     */
    TapeIndexRootNode();

    /**
     * Determine whether the node is an instance of the given node type.
     *
     * @param type The type to compare against.
     */
    virtual bool isType( int type );

    /**
     * Create child nodes for each tape index.
     *
     * @param expand This will always be set to TRUE.
     */
    virtual void expanding( bool expand=TRUE );

    /**
     * Change the node's pixmap to an open folder.
     */
    virtual void expanded();

    /**
     * Change the node's pixmap to a closed folder.
     */
    virtual void collapsed();
public slots:
    /**
     * Add a new child node for the new tape index.
     *
     * @param tape A pointer to the new tape index.
     */
    void slotTapeAdded( Tape* tape );

    /**
     * Remove the child associated with the tape index.
     *
     * @param tape A pointer to the removed tape index.
     */
    void slotTapeRemoved( Tape* tape );

    /**
     * Locate the child associated with the modified tape index, and make sure
     * that the displayed information (including all children) is updated.
     *
     * @param tape A pointer to the tape index that was modified.
     */
    void slotTapeModified( Tape* tape );
};

/**
 * @short This node represents a backup profile.
 */
class BackupProfileNode : public Node {
    BackupProfile* _backupProfile;
public:
    /**
     * Create a backup profile node.
     *
     * @param backupProfile A pointer to the backup profile that this node represents.
     */
    BackupProfileNode( BackupProfile* backupProfile );

    /**
     * Get the backup profile associated with this node.
     *
     * @return A pointer to the backup profile.
     */
    BackupProfile* getBackupProfile();

    /**
     * Make sure that the displayed information matches the backup profile.
     *
     * @return TRUE if the node's text has changed.
     */
    bool validate();

    /**
     * Determine whether the node is an instance of the given node type.
     *
     * @param type The type to compare against.
     */
    virtual bool isType( int type );

    /**
     * This method is called immediately after the node has been selected.
     */
    virtual void selected();

    /**
     * This method is called when the user right-clicks the mouse over the
     * node.
     */
    virtual void popupMenu( const QPoint& );
};

/**
 * @short This node represents the root of the backup profile subtree.
 */
class BackupProfileRootNode : public QObject, public Node {
    Q_OBJECT

    BackupProfileNode* findBackupProfileNode( BackupProfile* backupProfile );
public:
    /**
     * Create a backup profile root.
     */
    BackupProfileRootNode();

    /**
     * Select the node associated with the given backu profile.
     *
     * @param pBackupProfile The backup profile to select.
     */
    void setSelected( BackupProfile* pBackupProfile );

    /**
     * Determine whether the node is an instance of the given node type.
     *
     * @param type The type to compare against.
     */
    virtual bool isType( int type );

    /**
     * Create child nodes for each tape index.
     *
     * @param expand This will always be set to TRUE.
     */
    virtual void expanding( bool expand=TRUE );

    /**
     * Change the node's pixmap to an open folder.
     */
    virtual void expanded();

    /**
     * Change the node's pixmap to a closed folder.
     */
    virtual void collapsed();

    /**
     * This method is called when the user right-clicks the mouse over the
     * node.
     */
    virtual void popupMenu( const QPoint& );
public slots:
    /**
     * Add a new child node for the new backup profile.
     *
     * @param tape A pointer to the new backup profile.
     */
    void slotBackupProfileAdded( BackupProfile* backupProfile );

    /**
     * Remove the child associated with the backup profile.
     *
     * @param tape A pointer to the removed backup profile.
     */
    void slotBackupProfileRemoved( BackupProfile* backupProfile );

    /**
     * Locate the child associated with the modified backup profile, and make
     * sure that the displayed information is updated.
     *
     * @param tape A pointer to the backup profile that was modified.
     */
    void slotBackupProfileModified( BackupProfile* backupProfile );
};

#endif
