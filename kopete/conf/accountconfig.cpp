/*
    accountconfig.cpp  -  Kopete account config page

    Copyright (c) 2003 by Olivier Goffart <ogoffart@tiscalinet.be>

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/

#include "accountconfig.h"

#include <qpushbutton.h>
#include <qlistview.h>
#include <qlayout.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kdialogbase.h>

#include "kopeteprotocol.h"
#include "kopeteaccount.h"
#include "addaccountwizard.h"
#include "accountconfigbase.h"
#include "editaccountwidget.h"
#include "kopeteaccountmanager.h"

AccountConfig::AccountConfig(QWidget * parent) :
	ConfigModule (i18n("Accounts"),i18n("Here You Can Manage Your Accounts"),"personal", parent )
{
	QVBoxLayout *topLayout = new QVBoxLayout( this );
	m_view=new AccountConfigBase(this);
	topLayout->add(m_view);

	m_view->mButtonUp->setPixmap( SmallIcon("up") );
	m_view->mButtonDown->setPixmap( SmallIcon("down") );
	m_view->mAccountList->setSorting( -1 );

	connect( m_view->mButtonNew , SIGNAL(clicked() ) , this , SLOT( slotAddAccount()));
	connect( m_view->mButtonEdit , SIGNAL(clicked() ) , this , SLOT( slotEditAccount()));
	connect( m_view->mButtonRemove , SIGNAL(clicked() ) , this , SLOT( slotRemoveAccount()));
	connect( m_view->mAccountList , SIGNAL(selectionChanged() ) , this , SLOT( slotItemSelected()));
	connect( m_view->mButtonUp , SIGNAL(clicked() ) , this , SLOT( slotAccountUp()));
	connect( m_view->mButtonDown , SIGNAL(clicked() ) , this , SLOT( slotAccountDown()));

	slotItemSelected();
//	m_addwizard=0L;
}

AccountConfig::~AccountConfig()
{
}

void AccountConfig::save()
{
	KopeteAccountManager::manager()->save();
}

void AccountConfig::reopen()
{
	m_view->mAccountList->clear();
	m_accountItems.clear();

	QPtrList<KopeteAccount>  accounts = KopeteAccountManager::manager()->accounts();
	for(KopeteAccount *i=accounts.first() ; i; i=accounts.next() )
	{
		QListViewItem* lvi= new QListViewItem(m_view->mAccountList);
		lvi->setText(0,i->protocol()->displayName() + QString::fromLatin1(" ") );
		lvi->setPixmap( 0, SmallIcon( i->protocol()->pluginIcon() ) );
		lvi->setText(1, i->accountId());
		m_accountItems.insert(lvi,i);
	}
}

void AccountConfig::slotItemSelected()
{
	QListViewItem *itemSelected = m_view->mAccountList->selectedItem();

	m_view->mButtonEdit->setEnabled( itemSelected );
	m_view->mButtonRemove->setEnabled( itemSelected );

	if( itemSelected )
	{
		m_view->mButtonUp->setEnabled( itemSelected->itemAbove() );
		m_view->mButtonDown->setEnabled( itemSelected->itemBelow() );
	}
	else
	{
		m_view->mButtonUp->setEnabled( itemSelected );
		m_view->mButtonDown->setEnabled( itemSelected );
	}
}

void AccountConfig::slotAccountUp()
{
	QListViewItem *itemSelected = m_view->mAccountList->selectedItem();
	itemSelected->itemAbove()->moveItem( itemSelected );

	slotItemSelected();
}

void AccountConfig::slotAccountDown()
{
	QListViewItem *itemSelected = m_view->mAccountList->selectedItem();
	itemSelected->moveItem( itemSelected->itemBelow() );

	slotItemSelected();
}

void AccountConfig::slotAddAccount()
{
	AddAccountWizard *m_addwizard;
	m_addwizard= new AddAccountWizard( this , "addAccountWizard" , true);
	connect(m_addwizard, SIGNAL( destroyed(QObject*)) , this, SLOT (slotAddWizardDone()));
	m_addwizard->show();
}

void AccountConfig::slotEditAccount()
{
	QListViewItem *lvi=m_view->mAccountList->selectedItem();
	if(!lvi)
		return;

	KopeteAccount *ident=m_accountItems[lvi];
	KopeteProtocol *proto=ident->protocol();
	
	KDialogBase *editDialog= new KDialogBase( this,"editDialog", true, i18n( "Edit Account" ),
				KDialogBase::Ok|KDialogBase::Cancel, KDialogBase::Ok, true );
	editDialog->resize( 525, 400 );
	EditAccountWidget *m_accountWidget = proto->createEditAccountWidget(ident,editDialog);
	QWidget *qWidget = dynamic_cast<QWidget*>( m_accountWidget );

	if (!m_accountWidget)
		return;

	editDialog->setMainWidget(qWidget);
	if(editDialog->exec() == QDialog::Accepted )
	{
		if(m_accountWidget->validateData())
		{
			m_accountWidget->apply();
		}
	}
	editDialog->deleteLater();
	reopen();
}

void AccountConfig::slotRemoveAccount()
{
	QListViewItem *lvi=m_view->mAccountList->selectedItem();
	if(lvi)
	{
		KopeteAccount *i=m_accountItems[lvi];
		if( KMessageBox::warningContinueCancel( this, i18n("Are you sure you want to remove the account \"%1\"?").arg( i->accountId() ),
			 i18n("Remove Account"), i18n("Remove Account")) == KMessageBox::Continue )
		{
			m_accountItems.remove(lvi);
			i->deleteLater();
			delete lvi;
		}
	}
}

void AccountConfig::slotAddWizardDone()
{
	reopen();
}

#include "accountconfig.moc"


