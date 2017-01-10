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

#ifndef _ImageCache_h_
#define _ImageCache_h_

class QPixmap;

/**
 * @short A container for commonly used icons.
 */
class ImageCache {
    static ImageCache* _instance;

    QPixmap* _archive;
    QPixmap* _backup;
    QPixmap* _file;
    QPixmap* _folderClosed;
    QPixmap* _folderOpen;
    QPixmap* _restore;
    QPixmap* _selectAll;
    QPixmap* _selectNone;
    QPixmap* _selectSome;
    QPixmap* _tape;
    QPixmap* _tapeMounted;
    QPixmap* _tapeUnmounted;
    QPixmap* _verify;

    ImageCache();
public:
    /**
     * Destroy the image cache and free the icons.
     */
    ~ImageCache();

    /**
     * This method is an accessor for the single instance of the image cache.
     *
     * @return A pointer to the single instance of the image cache.
     */
    static ImageCache* instance();

    /**
     * Get the tree-node icon used for archives.
     *
     * @return A pointer to the icon.
     */
    const QPixmap* getArchive();

    /**
     * Get the toolbar icon used for the backup action.
     *
     * @return A pointer to the icon.
     */
    const QPixmap* getBackup();

    /**
     * Get the tree-node icon used for files.
     *
     * @return A pointer to the icon.
     */
    const QPixmap* getFile();

    /**
     * Get the tree-node icon used for a closed folder.
     *
     * @return A pointer to the icon.
     */
    const QPixmap* getFolderClosed();

    /**
     * Get the tree-node icon used for an open folder.
     *
     * @return A pointer to the icon.
     */
    const QPixmap* getFolderOpen();

    /**
     * Get the toolbar icon used for the restore action.
     *
     * @return A pointer to the icon.
     */
    const QPixmap* getRestore();

    /**
     * Get the tree-node icon used when all subnodes are selected.
     *
     * @return A pointer to the icon.
     */
    const QPixmap* getSelectAll();

    /**
     * Get the tree-node icon used when no subnodes are selected.
     *
     * @return A pointer to the icon.
     */
    const QPixmap* getSelectNone();
    
    /**
     * Get the tree-node icon used when some subnodes are selected.
     *
     * @return A pointer to the icon.
     */
    const QPixmap* getSelectSome();

    /**
     * Get the tree-node icon used for tape indexes.
     *
     * @return A pointer to the icon.
     */
    const QPixmap* getTape();

    /**
     * Get the tree-node/toolbar icon used for a mounted tape drive.
     *
     * @return A pointer to the icon.
     */
    const QPixmap* getTapeMounted();

    /**
     * Get the tree-node/toolbar icon used for an unmounted tape drive.
     *
     * @return A pointer to the icon.
     */
    const QPixmap* getTapeUnmounted();

    /**
     * Get the toolbar icon used for the verify action.
     *
     * @return A pointer to the icon.
     */
    const QPixmap* getVerify();
};

#endif
