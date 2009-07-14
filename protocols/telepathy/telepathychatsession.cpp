/*
 * This file is part of Kopete
 *
 * Copyright (C) 2009 Collabora Ltd. <http://www.collabora.co.uk/>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "telepathychatsession.h"
#include "telepathyprotocol.h"
#include "common.h"

#include <kopetechatsessionmanager.h>

#include <kdebug.h>

#include <TelepathyQt4/Contact>
#include <TelepathyQt4/Connection>
#include <TelepathyQt4/ContactManager>
#include <TelepathyQt4/PendingChannel>

TelepathyChatSession::TelepathyChatSession(const Kopete::Contact *user, Kopete::ContactPtrList others, Kopete::Protocol *protocol)
        : Kopete::ChatSession(user, others, protocol)
{
    kDebug(TELEPATHY_DEBUG_AREA);
    Kopete::ChatSessionManager::self()->registerChatSession(this);

    QObject::connect(this, SIGNAL(messageSent(Kopete::Message&, Kopete::ChatSession*)), this, SLOT(sendMessage(Kopete::Message&)));
    QObject::connect(this, SIGNAL(closing(Kopete::ChatSession *)), this, SLOT(closingChatSession(Kopete::ChatSession *)));
}

TelepathyChatSession::~TelepathyChatSession()
{
    kDebug(TELEPATHY_DEBUG_AREA);
}

void TelepathyChatSession::createTextChannel(QSharedPointer<Tp::Contact> contact)
{
    kDebug(TELEPATHY_DEBUG_AREA);
    m_contact = contact;

    Tp::ConnectionPtr connection = contact->manager()->connection();

    QVariantMap request;
    request.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType"),
                   TELEPATHY_INTERFACE_CHANNEL_TYPE_TEXT);
    request.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetHandleType"),
                   Tp::HandleTypeContact);
    request.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetHandle"),
                   connection->selfHandle());

    QObject::connect(connection->createChannel(request),
                     SIGNAL(finished(Tp::PendingOperation*)),
                     this,
                     SLOT(createChannelFinished(Tp::PendingOperation*)));
}

void TelepathyChatSession::sendMessage(Kopete::Message &message)
{
    kDebug(TELEPATHY_DEBUG_AREA);

    if (!m_textChannel)
        return;

    m_textChannel->send(message.plainBody());
}

void TelepathyChatSession::createChannelFinished(Tp::PendingOperation* operation)
{
    kDebug(TELEPATHY_DEBUG_AREA);

    if (TelepathyCommons::isOperationError(operation))
        return;

    Tp::PendingChannel *pc = qobject_cast<Tp::PendingChannel*>(operation);

    if (!pc) {
        kDebug(TELEPATHY_DEBUG_AREA) << "Error PendingChannel casting!";
        return;
    }

    //m_textChannel = pc->channel();

    QObject::connect(m_textChannel.data(),
                     SIGNAL(messageSent(const Tp::Message &, Tp::MessageSendingFlags, const QString &)),
                     this,
                     SLOT(messageSent(const Tp::Message &, Tp::MessageSendingFlags, const QString &)));
    QObject::connect(m_textChannel.data(),
                     SIGNAL(messageReveiced(const Tp::ReceivedMessage &)),
                     this,
                     SLOT(messageReveiced(const Tp::ReceivedMessage &)));
    QObject::connect(m_textChannel.data(),
                     SIGNAL(pendingMessageReceived(const Tp::ReceivedMessage &)),
                     this,
                     SLOT(pendingMessageReceived(const Tp::ReceivedMessage &)));
}

void TelepathyChatSession::closingChatSession(Kopete::ChatSession *kmm)
{
    kDebug(TELEPATHY_DEBUG_AREA);

    Q_UNUSED(kmm);

    if (!m_textChannel)
        return;

    QObject::connect(m_textChannel->requestClose(),
                     SIGNAL(finished(Tp::PendingOperation*)),
                     this,
                     SLOT(chatSessionRequestClose(Tp::PendingOperation*)));
}

void TelepathyChatSession::chatSessionRequestClose(Tp::PendingOperation *operation)
{
    kDebug(TELEPATHY_DEBUG_AREA);

    if (TelepathyCommons::isOperationError(operation))
        return;

    kDebug(TELEPATHY_DEBUG_AREA) << "Chat session closed";
}

void TelepathyChatSession::messageSent(const Tp::Message &message, Tp::MessageSendingFlags flags, const QString &sentMessageToken)
{
    kDebug(TELEPATHY_DEBUG_AREA);

    Kopete::Message::MessageType messageType = Kopete::Message::TypeNormal;
    /*
     if(message.messageType() == Tp::ChannelTypeMessageAction)
     {
      messageType = Kopete::Message::TypeAction;
     }
    */
    Kopete::Message newMessage(myself(), members());
    newMessage.setPlainBody(message.text());
    newMessage.setDirection(Kopete::Message::Outbound);
    newMessage.setType(messageType);

    appendMessage(newMessage);
    messageSucceeded();
}

void TelepathyChatSession::messageReceived(const Tp::ReceivedMessage &message)
{
    kDebug(TELEPATHY_DEBUG_AREA);

    Kopete::Message::MessageType messageType = Kopete::Message::TypeNormal;

    Kopete::Message newMessage(members().first(), myself());
    newMessage.setPlainBody(message.text());
    newMessage.setDirection(Kopete::Message::Inbound);
    newMessage.setType(messageType);

    appendMessage(newMessage);
}

void TelepathyChatSession::pendingMessageRemoved(const Tp::ReceivedMessage &message)
{
}
