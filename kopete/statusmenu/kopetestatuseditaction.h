/*
    kopetestatuseditaction.h - Kopete Status Edit Action

    Copyright (c) 2008      by Roman Jarosz          <kedgedev@centrum.cz>
    Kopete    (c) 2008      by the Kopete developers <kopete-devel@kde.org>

    *************************************************************************
    *                                                                       *
    * This library is free software; you can redistribute it and/or         *
    * modify it under the terms of the GNU Lesser General Public            *
    * License as published by the Free Software Foundation; either          *
    * version 2 of the License, or (at your option) any later version.      *
    *                                                                       *
    *************************************************************************
*/
#ifndef KOPETESTATUSEDITACTION_H
#define KOPETESTATUSEDITACTION_H

#include <QWidgetAction>

#include <KDialog>
#include <QObject>
#include "kopetestatusmenu_export.h"

class KDialogButtonBox;

namespace Ui {
class KopeteStatusEditWidget;
}

namespace Kopete {
class StatusMessage;

namespace UI {
class StatusEditWidget : public QWidget
{
    Q_OBJECT
public:
    StatusEditWidget(QWidget *parent = nullptr);
    ~StatusEditWidget();

    KDialogButtonBox *buttonBox() const;

    /**
     * Returns Kopete::StatusMessage
     **/
    Kopete::StatusMessage statusMessage() const;

    /**
     * Set status message to @p statusMessage
     **/
    void setStatusMessage(const Kopete::StatusMessage &statusMessage);

Q_SIGNALS:
    /**
     * This signal emitted after status message was changed
     **/
    void statusChanged(const Kopete::StatusMessage &statusMessage);

protected:
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void changeClicked();
    void clearClicked();

private:
    Ui::KopeteStatusEditWidget *ui;
};

class KOPETESTATUSMENU_EXPORT StatusEditAction : public QWidgetAction
{
    Q_OBJECT
public:
    /**
     * StatusEditAction constructor
     * Action for changing status title and message
     **/
    StatusEditAction(QObject *parent);

    /**
     * Returns Kopete::StatusMessage
     **/
    Kopete::StatusMessage statusMessage() const;

    /**
     * Set status message to @p statusMessage
     **/
    void setStatusMessage(const Kopete::StatusMessage &statusMessage);

Q_SIGNALS:
    /**
     * This signal emitted after status message was changed
     **/
    void statusChanged(const Kopete::StatusMessage &statusMessage);

private Q_SLOTS:
    void hideMenu();

private:
    StatusEditWidget *mStatusEditWidget;
};

class StatusEditDialog : public KDialog
{
public:
    StatusEditDialog(QWidget *parent = nullptr);

    /**
     * Returns Kopete::StatusMessage
     **/
    Kopete::StatusMessage statusMessage() const;

    /**
     * Set status message to @p statusMessage
     **/
    void setStatusMessage(const Kopete::StatusMessage &statusMessage);

private:
    StatusEditWidget *mStatusEditWidget;
};
}
}
#endif
