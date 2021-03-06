/*
 * This file is part of Embedded-IDE
 * 
 * Copyright 2019 Martin Ribelotta <martinribelotta@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "findlineedit.h"

#include <QHBoxLayout>
#include <QMenu>
#include <QToolButton>
#include <QStyle>
#include <QEvent>

static const QString INNER_BUTTON_STYLE = "background: transparent; border: none;";

FindLineEdit::FindLineEdit(QWidget *parent) : QLineEdit(parent)
{
    auto layout = new QHBoxLayout(this);
    auto clearButton = new QToolButton(this);
    optionsButton = new QToolButton(this);
    optionsButton->setIcon(QIcon(":/images/actions/application-menu.svg"));
    clearButton->setIcon(QIcon(":/images/actions/edit-clear.svg"));
    optionsButton->setFocusPolicy(Qt::NoFocus);
    optionsButton->setPopupMode(QToolButton::InstantPopup);
    optionsButton->setCursor(Qt::ArrowCursor);
    clearButton->setFocusPolicy(Qt::NoFocus);
    clearButton->setCursor(Qt::ArrowCursor);
    optionsButton->setStyleSheet(INNER_BUTTON_STYLE);
    clearButton->setStyleSheet(INNER_BUTTON_STYLE);
    int frameWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    setStyleSheet(QString("QLineEdit { padding-right: %1px; padding-left: %2px }\n"
                          "QToolButton::menu-indicator { image: none; }")
                  .arg(clearButton->sizeHint().width() + frameWidth + 1)
                  .arg(optionsButton->sizeHint().width() + frameWidth + 1));
    layout->addWidget(optionsButton, 0, Qt::AlignLeft);
    layout->addWidget(clearButton, 0, Qt::AlignRight);
    layout->setMargin(0);
    const auto margin = frameWidth * 2 + 2;
    layout->setContentsMargins(margin, 0, margin, 0);
    layout->setSpacing(0);
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));
    QSize msz = minimumSizeHint();
    setMinimumSize(qMax(msz.width(), clearButton->sizeHint().height() + margin),
                   qMax(msz.height(), clearButton->sizeHint().height() + margin));
}

void FindLineEdit::addMenuActions(const QHash<QString, QString> &actionList)
{
    auto menu = new QMenu(this);
    QHash<QString, QString>::const_iterator it;
    for (it = actionList.constBegin(); it != actionList.constEnd(); ++it) {
        const auto& actionName = it.key();
        const auto& propertyName = it.value();
        auto action = menu->addAction(actionName);
        action->setCheckable(true);
        setProperty(propertyName.toLatin1().constData(), false);
        actionMap.insert(propertyName, action);
        connect(action, &QAction::triggered, [this, propertyName](bool ck) {
            setProperty(propertyName.toLatin1().constData(), QVariant(ck));
            emit menuActionClicked(propertyName, ck);
        });
    }
    optionsButton->setMenu(menu);
}

void FindLineEdit::setPropertyChecked(const QString& propertyName, bool state)
{
    setProperty(propertyName.toLatin1().constData(), state);
    if (actionMap.contains(propertyName))
        actionMap.value(propertyName)->setChecked(state);
}
