/*
#
# Friction - https://friction.graphics
#
# Copyright (c) Friction contributors
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# See 'README.md' for more information.
#
*/

// Fork of enve - Copyright (C) 2016-2020 Maurycy Liebner

#include "welcomedialog.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QPainter>
#include <QLabel>

#include "appsupport.h"

WelcomeDialog::WelcomeDialog(QMenu *recentMenu,
                             const std::function<void()> &newFunc,
                             const std::function<void()> &openFunc,
                             QWidget * const parent)
    : QWidget(parent)
{
    setObjectName(QString::fromUtf8("welcomeDialog"));

    const auto thisLay = new QVBoxLayout(this);

    const auto mainWid = new QWidget(this);
    const auto mainLay = new QHBoxLayout(mainWid);

    const auto sceneWid = new QWidget(this);
    const auto sceneLay = new QVBoxLayout(sceneWid);

    int logoSize = 96;
    const auto logoLabel = new QLabel(this);
    logoLabel->setOpenExternalLinks(false);
    logoLabel->setMinimumWidth(logoSize*2);
    logoLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    logoLabel->setText(QString::fromUtf8("<div style=\"margin: 0; padding: 0; text-align: center; font-weight: normal;\">"
                                         "<p style=\"margin: 0; padding: 0;\"><img src=\":/icons/hicolor/%2x%2/apps/%4.png\" width=\"%2\" height=\"%2\"></p>"
                                         "<h1 style=\"font-weight: normal; margin-top: 0; padding-top: 0;\">%3<br><span style=\"font-size: large;\">%1</span></h1>"
                                         "</div>")
                                        .arg(AppSupport::getAppVersion(false),
                                             QString::number(logoSize),
                                             AppSupport::getAppDisplayName(),
                                             AppSupport::getAppName()));

    const auto buttonWid = new QWidget(this);
    buttonWid->setContentsMargins(0, 0, 0, 0);
    const auto buttonLay = new QHBoxLayout(buttonWid);
    buttonLay->setMargin(0);

    const auto newButton = new QPushButton(tr("New"), this);
    newButton->setObjectName("WelcomeButton");
    newButton->setSizePolicy(QSizePolicy::Preferred,
                             QSizePolicy::Expanding);
    connect(newButton, &QPushButton::released, newFunc);

    const auto openButton = new QPushButton(this);
    openButton->setObjectName("WelcomeButton");
    openButton->setText(tr("Open"));
    openButton->setSizePolicy(QSizePolicy::Preferred,
                              QSizePolicy::Expanding);
    connect(openButton, &QPushButton::released, openFunc);

    const auto recentButton = new QPushButton(tr("Open Recent"), this);
    recentButton->setSizePolicy(QSizePolicy::Preferred,
                                QSizePolicy::Preferred);
    recentButton->setContentsMargins(0, 0, 0, 0);
    recentButton->setObjectName("WelcomeRecentButton");
    recentButton->setMenu(recentMenu);

    thisLay->addWidget(mainWid, 0, Qt::AlignHCenter | Qt::AlignVCenter);
    sceneLay->addWidget(logoLabel);
    sceneLay->addWidget(buttonWid);
    sceneLay->addWidget(recentButton);
    buttonLay->addWidget(newButton);
    buttonLay->addWidget(openButton);
    mainLay->addWidget(sceneWid);
}

void WelcomeDialog::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(0, 0, width(), height(), Qt::black);
    p.end();
}
