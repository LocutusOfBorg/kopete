/*
  kopeteawaydialog.h  -  Kopete Away Dialog

  Copyright (c) 2002 by Hendrik vom Lehn <hvl@linux-4-ever.de>

  Kopete    (c) 2002 by the Kopete developers  <kopete-devel@kde.org>

  *************************************************************************
  *                                                                       *
  * This program is free software; you can redistribute it and/or modify  *
  * it under the terms of the GNU General Public License as published by  *
  * the Free Software Foundation; either version 2 of the License, or     *
  * (at your option) any later version.                                   *
  *                                                                       *
  *************************************************************************
  */

#ifndef KOPETEAWAYDIALOG_H
#define KOPETEAWAYDIALOG_H

class QString;
class KopeteAway;
class QKeyEvent;

#include <qstring.h>
#include "kopeteawaydialogbase.h"
   
/**
 * KopeteAwayDialog is a base class used for implementing
 * Away Message selection dialogs in Kopete.  It presents
 * the user with a list of pre-written away messages and
 * a line edit for them to type a "single shot" away message,
 * one that is not saved and will be lost the next time
 * they restart the application.
 *
 * Individual Protocols should subclass this class for protocol
 * specific Away Message choosers (in the case that the user
 * wants to set only one protocol away).  There are methods for
 * getting the message that the user selected, as well as a
 * virtual method that should be implemented that is called
 * when the user selects "OK", and should be used to do
 * protocol specific actions needed to set the user as
 * "Away" (or whatever the protocol calls it).
 *
 * @author Hendrik vom Lehn <hvl@linux-4-ever.de>
 * @author Christopher TenHarmsel <tenharmsel@users.sourceforge.net>
 */

class KopeteAwayDialog : public KopeteAwayBase
{
    Q_OBJECT

public:
    /**
     * Constructor for the Away Dialog
     * @param parent The object that owns this
     * @param name Name for this object
     */
    KopeteAwayDialog(QWidget *parent=0, const char *name=0);

    /**
     * Destructor
     */
    virtual ~KopeteAwayDialog();
	
protected:
    /**
     * Do not delete this, this instance will
     * deleted when the application closes
     */
    KopeteAway *awayInstance;

    /**
     * Gets the away message the user last
     * selected.
     * @return An away message
     */
    QString getSelectedAwayMessage();

    /**
     * This method is called when the user clicks
     * OK in the GUI, signalling that they wish
     * to set the away message that they chose.
     * Please reimplement this method to do protocol
     * specific things, and use getSelectedAwayMessage()
     * to get the text of the message that the user
     * selected.
     *
     * @param awayType This is the away type specified
     * if show was called with a parameter, if show() was called
     * instead, this parameter will be the empty string. You
		 * will need to compare it to an enum that you declare
		 * in your subclass.
     */
    virtual void setAway(QString /*awayType*/){}

    /**
     * This method is called when the user clicks
     * Cancel in the GUI, signalling that they
     * canceled their request to mark themselves as
     * away.  If your implementation finds this
     * information useful, implement this method
     * to handle this info.  By default it does nothing
     *     
     * @param awayType This is the away type specified
     * if show was called with a parameter, if show() was called
     * instead, this parameter will be the empty string.
     */
    virtual void cancelAway(QString /*awayType*/){}

private:
    /**
     * Initializes the GUI elements every time the
     * dialog is show.  Basically used for remembering
     * the singleshot message that the user may have
     * typed in.
     */
    void init();

    /**
     * The last user-entered away text
     * or the title of the last selected
     * saved away message, whichever was
     * last chosen
     */
    QString mLastUserAwayMessage;


		/**
		 * The last message that the user typed in the
		 * line edit
		 */
		QString mLastUserTypedMessage;
				
    /**
     * Flag indicating whether or not to use the users
     * written in away message
     */
    bool mUseSingleShot;

		/**
		 * This is used to store the type of away that we're
		 * going to go. 
		 */
		QString mExtendedAwayType;
		
public slots:
    /**
     * Shows the dialog
     */
    void show();

    /**
     * Shows the away dialog, but maintains a "state"
     * so you can specify if you're setting away,
     * do not disturb, gone, etc for protocols that
     * support this like ICQ and MSN.
     *
     * This string does not have any special internal
     * meaning, but rather will get passed to setAway()
     * when it is called so that you can decide what
     * kind of "away" you really want to do.
     *
     * @param status The type of "away" you want to set.
     */
    void show(QString status);

protected slots:
    /**
     * This slot is called when the user click on "OK"
     * it will call setAway(), which is pure virtual and
     * should be implemented for specific needs
     */
    void slotOkayClicked();

    /**
     * This slot is called when the user clicks on
     * "Cancel".  It calls cancelAway(), which is
     * pure virtual and should be implemented to
     * fit your specific needs if the user selects
     * "Cancel".  This method will close the
     * dialog, but if you require any specific actions
     * please implement them in cancelAway().
     */
    void slotCancelClicked();

private slots:
    /**
     * This is called when the user types in the
     * text field for inputting a custom away message
     */
    void slotSingleShotTextChanged(const QString &newString);
    
    /**
     * This is called when the user selects an
     * entry from the combo box
     */
    void slotComboBoxSelection(int index);
};

#endif
/*
 * Local variables:
 * mode: c++
 * c-indentation-style: k&r
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */

// vim: set noet ts=4 sts=4 sw=4:

