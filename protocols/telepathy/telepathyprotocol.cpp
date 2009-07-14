/*
 * telepathyprotocol.cpp - Telepathy protocol definition.
 *
 * Copyright (c) 2006 by Michaël Larouche <larouche@kde.org>
 *
 * Kopete    (c) 2002-2006 by the Kopete developers  <kopete-devel@kde.org>
 *
 *************************************************************************
 *                                                                       *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 *************************************************************************
 */

#include "telepathyprotocol.h"

// KDE includes
#include <kgenericfactory.h>
#include <kdebug.h>

// Kopete includes
#include <kopeteaccount.h>
#include <kopeteaccountmanager.h>
#include <kopetemetacontact.h>

#include "telepathyaccount.h"

#include <TelepathyQt4/Types>

// Local includes
#include "telepathyeditaccountwidget.h"
//#include "telepathyaddcontactpage.h"

K_PLUGIN_FACTORY(TelepathyProtocolFactory, registerPlugin<TelepathyProtocol>();)
K_EXPORT_PLUGIN(TelepathyProtocolFactory("kopete_telepathy"))

TelepathyProtocol *TelepathyProtocol::s_self = 0;

TelepathyProtocol::TelepathyProtocol(QObject *parent, const QVariantList &/*args*/)
        : Kopete::Protocol(TelepathyProtocolFactory::componentData(), parent),
        // Create Kopete::OnlineStatus
        Available(Kopete::OnlineStatus::Online, 25, this, 1, QStringList(),
                  i18n("Available"), i18n("A&vailable"), Kopete::OnlineStatusManager::Online, Kopete::OnlineStatusManager::HasStatusMessage),
        Away(Kopete::OnlineStatus::Away, 18, this, 4, QStringList(QString::fromLatin1("contact_away_overlay")),
             i18n("Away From Computer"), i18n("&Away"), Kopete::OnlineStatusManager::Away, Kopete::OnlineStatusManager::HasStatusMessage),
        Busy(Kopete::OnlineStatus::Away, 20, this, 2, QStringList(),
             i18n("Busy"), i18n("&Busy"), Kopete::OnlineStatusManager::Busy, Kopete::OnlineStatusManager::HasStatusMessage),
        Hidden(Kopete::OnlineStatus::Invisible, 3, this, 8, QStringList(QString::fromLatin1("contact_invisible_overlay")),
               i18n("Invisible"), i18n("&Hidden"), Kopete::OnlineStatusManager::Invisible),
        ExtendedAway(Kopete::OnlineStatus::Away, 15, this, 4, QStringList(QString::fromLatin1("contact_away_overlay")),
                     i18n("Extended Away"), i18n("&Extended Away"), Kopete::OnlineStatusManager::Away, Kopete::OnlineStatusManager::HasStatusMessage),
        Offline(Kopete::OnlineStatus::Offline, 0, this, 7, QStringList(),
                i18n("Offline"), i18n("&Offline"), Kopete::OnlineStatusManager::Offline,
                Kopete::OnlineStatusManager::DisabledIfOffline),
        propAvatarToken("telepathyAvatarToken", i18n("Telepathy Avatar token"), QString(), Kopete::PropertyTmpl::PersistentProperty | Kopete::PropertyTmpl::PrivateProperty)
{
    Tp::registerTypes();

    s_self = this;

    addAddressBookField("messaging/telepathy", Kopete::Plugin::MakeIndexField);
}

TelepathyProtocol *TelepathyProtocol::protocol()
{
    return s_self;
}

Kopete::Account *TelepathyProtocol::createNewAccount(const QString &accountId)
{
    kDebug(TELEPATHY_DEBUG_AREA) << "createNewAccount() called" << accountId;

    return new TelepathyAccount(this, accountId);
}

AddContactPage *TelepathyProtocol::createAddContactWidget(QWidget *parent, Kopete::Account *account)
{
    kDebug(TELEPATHY_DEBUG_AREA);
    Q_UNUSED(parent);
    Q_UNUSED(account);

// return new TelepathyAddContactPage(parent);
    return 0;
}

KopeteEditAccountWidget *TelepathyProtocol::createEditAccountWidget(Kopete::Account *account, QWidget *parent)
{
    kDebug(TELEPATHY_DEBUG_AREA);
    return new TelepathyEditAccountWidget(account, parent);
}
/*
Kopete::Contact *TelepathyProtocol::deserializeContact( Kopete::MetaContact *metaContact,
  const QMap<QString, QString> &serializedData, const QMap<QString, QString> &addressBookData )
{
    kDebug(TELEPATHY_DEBUG_AREA) << "deserializeContact() called";
 Q_UNUSED(addressBookData);

 QString contactId = serializedData["contactId"];
 QString accountId = serializedData["accountId"];

 // Find the account
 QList<Kopete::Account*> accounts = Kopete::AccountManager::self()->accounts( this );

 Kopete::Account *account = 0;
 foreach( account, accounts )
 {
  if(account->accountId() == accountId)
   break;
 }

 if( account )
 {
  return new TelepathyContact( static_cast<TelepathyAccount*>(account), contactId, metaContact);
 }

 return 0;
}
*/
QString TelepathyProtocol::formatTelepathyConfigGroup(const QString &connectionManager, const QString &protocol, const QString &accountId)
{
    return QString("Telepathy_%1_%2_%3").arg(connectionManager).arg(protocol).arg(accountId);
}

Tp::ConnectionPresenceType TelepathyProtocol::kopeteStatusToTelepathy(const Kopete::OnlineStatus &status)
{
    kDebug(TELEPATHY_DEBUG_AREA);
    Tp::ConnectionPresenceType telepathyPresence = Tp::ConnectionPresenceTypeOffline;

    Kopete::OnlineStatus type = status.status();

    if (type == Kopete::OnlineStatus::Online)
        telepathyPresence = Tp::ConnectionPresenceTypeAvailable;
    else if (status == Kopete::OnlineStatus::Away)
        telepathyPresence = Tp::ConnectionPresenceTypeAway;
    else if (status == Kopete::OnlineStatus::Invisible)
        telepathyPresence = Tp::ConnectionPresenceTypeHidden;
    else if (status == Kopete::OnlineStatus::Offline)
        telepathyPresence = Tp::ConnectionPresenceTypeOffline;
    else if (status == Kopete::OnlineStatus::Unknown)
        telepathyPresence = Tp::ConnectionPresenceTypeUnset;

    return telepathyPresence;
}

Kopete::OnlineStatus TelepathyProtocol::telepathyStatusToKopete(Tp::ConnectionPresenceType presence)
{
    kDebug(TELEPATHY_DEBUG_AREA) << "telepathyStatusToKopete() called";
    Kopete::OnlineStatus result;
    switch (presence) {
    case Tp::ConnectionPresenceTypeAvailable:
        result = Kopete::OnlineStatus::Online;
        break;
    case Tp::ConnectionPresenceTypeAway:
    case Tp::ConnectionPresenceTypeExtendedAway:
    case Tp::ConnectionPresenceTypeBusy:
        result = Kopete::OnlineStatus::Away;
        break;
    case Tp::ConnectionPresenceTypeHidden:
        result = Kopete::OnlineStatus::Invisible;
        break;
    case Tp::ConnectionPresenceTypeOffline:
        result = Kopete::OnlineStatus::Offline;
        break;
    case Tp::ConnectionPresenceTypeUnset:
    case Tp::ConnectionPresenceTypeUnknown:
    case Tp::ConnectionPresenceTypeError:
        result = Kopete::OnlineStatus::Unknown;
        break;
    }

    return result;
}


#include "telepathyprotocol.moc"
