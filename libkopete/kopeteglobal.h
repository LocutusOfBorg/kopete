/*
    kopeteglobal.h - Kopete Globals

    Copyright (c) 2004      by Richard Smith         <kde@metafoo.co.uk>

    Kopete    (c) 2004      by the Kopete developers <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU Lesser General Public            *
    * License as published by the Free Software Foundation; either          *
    * version 2 of the License, or (at your option) any later version.      *
    *                                                                       *
    *************************************************************************
*/

#ifndef KOPETEGLOBAL_H
#define KOPETEGLOBAL_H

#include <kurl.h>

class QString;

namespace Kopete
{

class MimeTypeHandler
{
protected:
	MimeTypeHandler( bool canAcceptRemoteFiles = false );
public:
	virtual ~MimeTypeHandler();

	/**
	 * Returns a list of mime types this object is registered to handle
	 */
	const QStringList mimeTypes() const;

	/**
	 * Returns true if this handler can accept remote files direcltly;
	 * If false, remote files are downloaded via KIO::NetAccess before
	 * being passed to handleURL
	 */
	bool canAcceptRemoteFiles() const;

	/**
	 * Handles the URL @p url, which has the mime type @p mimeType
	 *
	 * @param mimeType The mime type of the URL
	 * @param url The url to handle
	 */
	virtual void handleURL( const QString &mimeType, const KURL &url ) const = 0;

protected:
	/**
	 * Register this object as the handler of type @p mimeType.
	 * @param mimeType the mime type to handle
	 * @return true if registration succeeded, false if another handler is
	 *         already set for this mime type.
	 */
	bool registerAsHandler( const QString &mimeType );

private:
	class Private;
	Private *d;
};

/**
 * Mime-type handler class for Kopete emotiocon files
 */
class EmoticonHandler : public MimeTypeHandler
{
public:
	EmoticonHandler();

	const QStringList mimeTypes() const;

	/**
	 * \brief Installs one or more kopete emoticon themes from a tarball
	 * (either .kopete-emoticons or tar.gz or tar.bz2)
	 *
	 * @p archiveName Full path to a local emoticon archive, use KIO to download
	 * files in case their are non-local.
	 *
	 * @return true in case install was successful, false otherwise. Errors are
	 * displayed by either KIO or by using KMessagebox directly.
	 *
	 * TODO: If possible, port it to KIO instead of using ugly blocking KTar
	 **/
	void handleURL( const QString &mimeType, const KURL &url ) const;
};

/**
 * This namespace contains Kopete's global settings and functions
 */
namespace Global
{
	/**
	 * Handles a URL argument with a registered MimeTypeHandler.
	 *
	 * @param url The url passed to Kopete
	 *
	 * @return True if the type was handled, false otherwise
	 */
	bool handleURL( const KURL &url );
} // Global

} // Kopete

#endif
// vim: set noet ts=4 sts=4 sw=4:
