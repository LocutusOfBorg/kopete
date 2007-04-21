/*
 Kopete Oscar Protocol
 icqsearchdialog.h - search for people

 Copyright (c) 2005 Matt Rogers <mattr@kde.org>

 Kopete (c) 2002-2005 by the Kopete developers <kopete-devel@kde.org>

 *************************************************************************
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU Lesser General Public            *
 * License as published by the Free Software Foundation; either          *
 * version 2 of the License, or (at your option) any later version.      *
 *                                                                       *
 *************************************************************************
*/

#ifndef ICQSEARCHDIALOG_H
#define ICQSEARCHDIALOG_H

#include <kdialog.h>
#include "icquserinfo.h"

class ICQAccount;
class ICQContact;
class ICQUserInfoWidget;
class QStandardItemModel;
class QModelIndex;

namespace Ui
{
	class ICQUserInfoWidget;
	class ICQSearchBase;
}
/**
@author Kopete Developers
*/
class ICQSearchDialog : public KDialog
{
Q_OBJECT
public:
	explicit ICQSearchDialog( ICQAccount* account, QWidget* parent = 0 );
	~ICQSearchDialog();

private slots:
	void startSearch();
	void stopSearch();
	void addContact();
	void clearResults();
	void closeDialog();
	void userInfo();
	void closeUserInfo();
	void newSearch();

	/// Enable/disable buttons when the selection changes
	void resultRowChanged( const QModelIndex& current );
	
	/// Add a search result to the listview
	void newResult( const ICQSearchResult& info );
	
	/// The search is finished
	void searchFinished( int numLeft );

private:
	ICQAccount* m_account;
	Ui::ICQSearchBase* m_searchUI;
	ICQContact* m_contact;
	ICQUserInfoWidget* m_infoWidget;
	QStandardItemModel* m_searchResultsModel;
	
	void clearFields();
};

#endif

//kate: indent-mode csands; space-indent off; replace-tabs off; tab-width 4;
