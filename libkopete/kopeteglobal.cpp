/*
    kopeteglobal.cpp - Kopete Globals

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

#include "kopeteglobal.h"
#include "kopeteuiglobal.h"

#include <qwidget.h>

#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprogress.h>
#include <kstandarddirs.h>
#include <ktar.h>

namespace Kopete
{

bool Global::installTheme(const QString &archiveName)
{
	QStringList foundThemes;
	KArchiveEntry *currentEntry = 0L;
	KArchiveDirectory* currentDir = 0L;
	KProgressDialog *progressDlg = 0L;
	KTar *archive = 0L;

	QString localThemesDir(locateLocal("data",
		QString::fromLatin1("kopete/pics/emoticons/")));

	if(localThemesDir.isEmpty())
	{
		KMessageBox::queuedMessageBox(Kopete::UI::Global::mainWidget(),
			KMessageBox::Error, i18n("Could not find suitable place " \
			"to install emoticon themes into."));
		return false;
	}

	progressDlg = new KProgressDialog(0 , "emoticonInstProgress",
	 	i18n("Installing Emoticon Themes..."), QString::null, true);
	progressDlg->progressBar()->setTotalSteps(foundThemes.count());
	progressDlg->show();
	kapp->processEvents();

	archive = new KTar(archiveName);
	if(archive->open(IO_ReadOnly) == false)
	{
		KMessageBox::queuedMessageBox(Kopete::UI::Global::mainWidget(),
			KMessageBox::Error,
			i18n("Could not open \"%1\" for unpacking.").arg(archiveName));
		delete archive;
		delete progressDlg;
		return false;
	}

	const KArchiveDirectory* rootDir = archive->directory();

	// iterate all the dirs looking for an emoticons.xml file
	QStringList entries = rootDir->entries();
	for (QStringList::Iterator it = entries.begin(); it != entries.end(); ++it)
	{
		currentEntry = const_cast<KArchiveEntry*>(rootDir->entry(*it));
		if (currentEntry->isDirectory())
		{
			currentDir = dynamic_cast<KArchiveDirectory*>( currentEntry );
			if (currentDir && (currentDir->entry(QString::fromLatin1("emoticons.xml")) != NULL))
				foundThemes.append(currentDir->name());
		}
	}

	if (foundThemes.isEmpty())
	{
		KMessageBox::queuedMessageBox(Kopete::UI::Global::mainWidget(),
			KMessageBox::Error, i18n("<qt>The file \"%1\" is not a valid" \
				" emoticon theme archive!</qt>").arg(archiveName));
		archive->close();
		delete archive;
		delete progressDlg;
		return false;
	}

	for (QStringList::ConstIterator it = foundThemes.begin(); it != foundThemes.end(); ++it)
	{
		progressDlg->setLabel(
			i18n("<qt>Installing <strong>%1</strong> emoticon theme</qt>")
			.arg(*it));
		progressDlg->resize(progressDlg->sizeHint());
		kapp->processEvents();

		if (progressDlg->wasCancelled())
			break;

		currentEntry = const_cast<KArchiveEntry *>(rootDir->entry(*it));
		if (currentEntry == 0)
		{
			kdDebug(14000) << k_funcinfo << "couldn't get next archive entry" << endl;
			continue;
		}

		if(currentEntry->isDirectory())
		{
			currentDir = dynamic_cast<KArchiveDirectory*>(currentEntry);
			if (currentDir == 0)
			{
				kdDebug(14000) << k_funcinfo <<
					"couldn't cast archive entry to KArchiveDirectory" << endl;
				continue;
			}
			currentDir->copyTo(localThemesDir + *it);
			progressDlg->progressBar()->advance(1);
		}
	}

	archive->close();
	delete archive;

	// check if all steps were done, if there are skipped ones then we didn't
	// succeed copying all dirs from the tarball
	if (progressDlg->progressBar()->totalSteps() !=
		progressDlg->progressBar()->progress())
	{
		KMessageBox::queuedMessageBox(Kopete::UI::Global::mainWidget(),
			KMessageBox::Error,
			i18n("<qt>A problem occurred during the installation process. "
			"However, most of the emoticon themes in the archive have been " \
			"installed</qt>"));
	}

	delete progressDlg;

	return true;
} // END installTheme()

} // END namespace Kopete

// vim: set noet ts=4 sts=4 sw=4:
