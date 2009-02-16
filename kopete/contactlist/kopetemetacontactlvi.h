/*
    kopetemetacontactlvi.h - Kopete Meta Contact K3ListViewItem

    Copyright (c) 2004      by Richard Smith          <kde@metafoo.co.uk>
    Copyright (c) 2002-2003 by Olivier Goffart        <ogoffart@kde.org>
    Copyright (c) 2002-2003 by Martijn Klingens       <klingens@kde.org>
    Copyright (c) 2002      by Duncan Mac-Vicar P     <duncan@kde.org>

    Kopete    (c) 2002-2003 by the Kopete developers  <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/

#ifndef __kopetemetacontactlvi_h__
#define __kopetemetacontactlvi_h__

#include "kopetelistviewitem.h"

#include <QtGui/QPixmap>

#include <k3listview.h>

class QVariant;


namespace Kopete
{
class Account;
class MetaContact;
class Contact;
class Group;
class MessageEvent;
}

class KopeteGroupViewItem;


/**
 * @author Martijn Klingens <klingens@kde.org>
 */
class KopeteMetaContactLVI : public Kopete::UI::ListView::Item
{
	Q_OBJECT

public:
	KopeteMetaContactLVI( Kopete::MetaContact *contact, KopeteGroupViewItem *parent );
	KopeteMetaContactLVI( Kopete::MetaContact *contact, Q3ListViewItem *parent );
	KopeteMetaContactLVI( Kopete::MetaContact *contact, Q3ListView *parent );
	~KopeteMetaContactLVI();

	/**
	 * metacontact this visual item represents
	 */
	Kopete::MetaContact *metaContact() const
	{ return m_metaContact; }

	/**
	 * true if the item is at top level and not under a group
	 */
	bool isTopLevel() const;

	/**
	 * parent when top-level
	 */
	Q3ListView *parentView() const { return m_parentView; }

	/**
	 * parent when not top-level
	 */
	KopeteGroupViewItem *parentGroup() const { return m_parentGroup; }

	/**
	 * call this when the item has been moved to a different group
	 */
	void movedToDifferentGroup();
	void rename( const QString& name );
	void startRename( int );

	Kopete::Group *group();

	/**
	 * Returns the Kopete::Contact of the small little icon at the point p
	 * @param p must be in the list view item's coordinate system.
	 * Returns a null pointer if p is not on a small icon.
	 * (This is used for e.g. the context-menu of a contact when
	 * right-clicking an icon, or the tooltips)
	 */
	Kopete::Contact *contactForPoint( const QPoint &p ) const;

	/**
	 * Returns the QRect small little icon used for the Kopete::Contact.
	 * The behavior is undefined if @param c doesn't point to a valid
	 * Kopete::Contact for this list view item.
	 * The returned QRect is using the list view item's coordinate
	 * system and should probably be transformed into the list view's
	 * coordinates before being of any practical use.
	 * Note that the returned Rect is always vertically stretched to fill
	 * the full list view item's height, only the width is relative to
	 * the actual icon width.
	 */
	QRect contactRect( const Kopete::Contact *c ) const;

	bool isGrouped() const;

	/**
	 * reimplemented from K3ListViewItem to take into account our alternate text storage
	 */
	virtual QString text( int column ) const;
	virtual void setText( int column, const QString &text );

public slots:
	/**
	 * Call the meta contact's execute as I don't want to expose m_contact
	 * directly.
	 */
	void execute() const;

	void catchEvent( Kopete::MessageEvent * );

	void updateVisibility();

private slots:
	void slotUpdateMetaContact();
	void slotContactStatusChanged( Kopete::Contact * );
	void slotContactPropertyChanged( Kopete::PropertyContainer *, const QString &, const QVariant &, const QVariant & );
	void slotContactAdded( Kopete::Contact * );
	void slotContactRemoved( Kopete::Contact * );

	void slotDisplayNameChanged();
	void slotPhotoChanged();

	void slotAddToNewGroup();
	void resetIdleTimeout();
	void idleStateChanged();
	void updateIdleState( Kopete::Contact* = 0L );

	void slotConfigChanged();

	void slotEventDone( Kopete::MessageEvent* );
	void slotBlink();

	void updateContactIcons();

protected:
	void okRename(int col);
	void cancelRename(int col);

private:
	void initLVI();
	void setDisplayMode();
	void setMetaContactToolTipSourceForComponent( Kopete::UI::ListView::Component *comp );
	QString key( int column, bool ascending ) const;
	void updateContactIcon( Kopete::Contact * );
	Kopete::UI::ListView::ContactComponent *contactComponent( const Kopete::Contact *c ) const;

	Kopete::MetaContact *m_metaContact;
	KopeteGroupViewItem *m_parentGroup;
	Q3ListView *m_parentView;
	bool m_isTopLevel;

	int m_pixelWide;

	Kopete::OnlineStatus m_oldStatus;
	QPixmap m_oldStatusIcon;
	QPixmap m_originalBlinkIcon;

	QTimer *mBlinkTimer;

	bool mIsBlinkIcon;
	int m_blinkLeft;

	class Private;
	Private * const d;
};

#endif

// vim: set noet ts=4 sts=4 sw=4:

