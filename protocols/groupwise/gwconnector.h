
/***************************************************************************
                   gwconnector.h  -  Socket Connector for KNetwork
                             -------------------
    begin                : Wed Jul 7 2004
    copyright            : (C) 2004 by Till Gerken <till@tantalo.net>

			   Kopete (C) 2004 Kopete developers <kopete-devel@kde.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef GWCONNECTOR_H
#define GWCONNECTOR_H

#include "gwbytestream.h"

#include "connector.h"

class ByteStream;
class KNetworkByteStream;

/**
@author Till Gerken
*/
class KNetworkConnector : public Connector
{

Q_OBJECT

public:
	KNetworkConnector ( QObject *parent = nullptr );

	virtual ~KNetworkConnector ();

	void connectToServer ( const QString &server ) Q_DECL_OVERRIDE;
	ByteStream *stream () const Q_DECL_OVERRIDE;
	void done () Q_DECL_OVERRIDE;

	void setOptHostPort ( const QString &host, quint16 port );
	void setOptSSL ( bool );

	int errorCode ();

private Q_SLOTS:
	void slotConnected ();
	void slotError ( int );

private:
	QString mHost;
	quint16 mPort;
	int mErrorCode;

	KNetworkByteStream *mByteStream;

};

#endif
