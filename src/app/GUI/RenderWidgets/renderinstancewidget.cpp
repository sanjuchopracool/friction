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

#include "renderinstancewidget.h"
#include "GUI/global.h"
#include <QMenu>
#include "outputsettingsdialog.h"
#include "outputsettingsprofilesdialog.h"
#include "outputsettingsdisplaywidget.h"
#include "rendersettingsdisplaywidget.h"
#include "Private/esettings.h"
#include "GUI/edialogs.h"

#include "appsupport.h"

#include <QDesktopServices>

RenderInstanceWidget::RenderInstanceWidget(
        Canvas *canvas, QWidget *parent) :
    ClosableContainer(parent), mSettings(canvas) {
    iniGUI();
    connect(&mSettings, &RenderInstanceSettings::stateChanged,
            this, &RenderInstanceWidget::updateFromSettings);
    updateFromSettings();
}

RenderInstanceWidget::RenderInstanceWidget(const RenderInstanceSettings& sett,
                                           QWidget *parent) :
    ClosableContainer(parent), mSettings(sett) {
    iniGUI();
    connect(&mSettings, &RenderInstanceSettings::stateChanged,
            this, &RenderInstanceWidget::updateFromSettings);
    updateFromSettings();
}

void RenderInstanceWidget::iniGUI() {
    if(!OutputSettingsProfile::sOutputProfilesLoaded) {
        OutputSettingsProfile::sOutputProfilesLoaded = true;
        const QString dirPath = AppSupport::getAppOutputProfilesPath();
        QDirIterator dirIt(dirPath, QDirIterator::NoIteratorFlags);
        while(dirIt.hasNext()) {
            const auto path = dirIt.next();
            const QFileInfo fileInfo(path);
            if(!fileInfo.isFile()) continue;
            if(!fileInfo.completeSuffix().contains("conf")) continue;
            const auto profile = enve::make_shared<OutputSettingsProfile>();
            try {
                profile->load(path);
            } catch(const std::exception& e) {
                gPrintExceptionCritical(e);
            }
            OutputSettingsProfile::sOutputProfiles << profile;
        }
    }

    setCheckable(true);
    setObjectName("darkWidget");
    mNameLabel = new QLineEdit(this);
    mNameLabel->setFocusPolicy(Qt::NoFocus);
    mNameLabel->setFixedHeight(eSizesUI::widget);
    mNameLabel->setObjectName("darkWidget");
    mNameLabel->setReadOnly(true);
    mNameLabel->setFrame(false);

    setLabelWidget(mNameLabel);

    QWidget *contWid = new QWidget(this);
    contWid->setContentsMargins(0, 0, 0, 0);
    contWid->setLayout(mContentLayout);
    contWid->setObjectName("darkWidget");

    addContentWidget(contWid);

    mRenderSettingsDisplayWidget = new RenderSettingsDisplayWidget(this);

    QWidget *renderSettingsLabelWidget = new QWidget(this);
    renderSettingsLabelWidget->setContentsMargins(0, 0, 0, 0);
    renderSettingsLabelWidget->setObjectName("darkWidget");
    QVBoxLayout *renderSettingsLayout = new QVBoxLayout(renderSettingsLabelWidget);

    mRenderSettingsButton = new QPushButton(tr("Scene ..."));
    mRenderSettingsButton->setFocusPolicy(Qt::NoFocus);
    mRenderSettingsButton->setObjectName("renderSettings");
    mRenderSettingsButton->setSizePolicy(QSizePolicy::Preferred,
                                        QSizePolicy::Preferred);
    connect(mRenderSettingsButton, &QPushButton::pressed,
            this, &RenderInstanceWidget::openRenderSettingsDialog);

    renderSettingsLayout->addWidget(mRenderSettingsButton);
    renderSettingsLayout->addWidget(mRenderSettingsDisplayWidget);

    mContentLayout->addWidget(renderSettingsLabelWidget);

    mOutputSettingsDisplayWidget = new OutputSettingsDisplayWidget(this);

    QWidget *outputSettingsLabelWidget = new QWidget(this);
    outputSettingsLabelWidget->setContentsMargins(0, 0, 0, 0);
    //outputSettingsLabelWidget->setObjectName("darkWidget");
    QVBoxLayout *outputSettingsLayout = new QVBoxLayout(outputSettingsLabelWidget);

    mOutputSettingsProfilesButton = new OutputProfilesListButton(this);
    mOutputSettingsProfilesButton->setObjectName("FlatButton");
    mOutputSettingsProfilesButton->setFocusPolicy(Qt::NoFocus);
    connect(mOutputSettingsProfilesButton, &OutputProfilesListButton::profileSelected,
            this, &RenderInstanceWidget::outputSettingsProfileSelected);

    mOutputSettingsButton = new QPushButton(tr("Output ..."));
    mOutputSettingsButton->setFocusPolicy(Qt::NoFocus);
    mOutputSettingsButton->setSizePolicy(QSizePolicy::Expanding,
                                         QSizePolicy::Preferred);
    connect(mOutputSettingsButton, &QPushButton::pressed,
            this, &RenderInstanceWidget::openOutputSettingsDialog);

    QWidget *outputSettingsOptWidget = new QWidget(this);
    outputSettingsOptWidget->setContentsMargins(0, 0, 0, 0);
    const auto outputSettingsOptLayout = new QHBoxLayout(outputSettingsOptWidget);
    outputSettingsOptLayout->setMargin(0);
    outputSettingsOptLayout->addWidget(mOutputSettingsProfilesButton);
    outputSettingsOptLayout->addWidget(mOutputSettingsButton);

    outputSettingsLayout->addWidget(outputSettingsOptWidget);
    outputSettingsLayout->addWidget(mOutputSettingsDisplayWidget);

    mOutputDestinationButton = new QPushButton(QIcon::fromTheme("dots"),
                                               QString(),
                                               this);
    mOutputDestinationButton->setObjectName("FlatButton");
    mOutputDestinationButton->setFocusPolicy(Qt::NoFocus);
    mOutputDestinationButton->setToolTip(tr("Select output file"));
    connect(mOutputDestinationButton, &QPushButton::pressed,
            this, &RenderInstanceWidget::openOutputDestinationDialog);

    mPlayButton = new QPushButton(QIcon::fromTheme("play"),
                                            QString(),
                                            this);
    mPlayButton->setObjectName("FlatButton");
    mPlayButton->setFocusPolicy(Qt::NoFocus);
    mPlayButton->setToolTip(tr("Open in default application"));
    connect(mPlayButton, &QPushButton::pressed,
            this, [this]() {
        QString dst = mOutputDestinationLineEdit->text();
        if (QFile::exists(dst)) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(dst));
        }
    });

    mOutputDestinationLineEdit = new QLineEdit(this);
    mOutputDestinationLineEdit->setFocusPolicy(Qt::NoFocus);
    mOutputDestinationLineEdit->setSizePolicy(QSizePolicy::Expanding,
                                              QSizePolicy::Preferred);
    mOutputDestinationLineEdit->setReadOnly(true);
    mOutputDestinationLineEdit->setPlaceholderText(tr("Destination ..."));
    mOutputDestinationLineEdit->setObjectName(QString::fromUtf8("OutputDestinationLineEdit"));

    eSizesUI::widget.add(mOutputSettingsProfilesButton, [this](const int size) {
        mRenderSettingsButton->setFixedHeight(size);
        mOutputSettingsButton->setFixedHeight(size);
        mOutputSettingsProfilesButton->setIconSize(QSize(size, size));
        mOutputSettingsProfilesButton->setFixedSize(QSize(size, size));
        mOutputDestinationButton->setIconSize(QSize(size, size));
        mOutputDestinationButton->setFixedSize(QSize(size, size));
        mPlayButton->setIconSize(QSize(size, size));
        mPlayButton->setFixedSize(QSize(size, size));
        mOutputDestinationLineEdit->setFixedHeight(size);
    });

    QWidget *outputDestinationWidget = new QWidget(this);
    outputDestinationWidget->setContentsMargins(0, 0, 0, 0);
    const auto outputDesinationLayout = new QHBoxLayout(outputDestinationWidget);
    outputDesinationLayout->setMargin(0);

    outputDesinationLayout->addWidget(mOutputDestinationButton);
    outputDesinationLayout->addWidget(mPlayButton);
    outputDesinationLayout->addWidget(mOutputDestinationLineEdit);

    outputSettingsLayout->addWidget(outputDestinationWidget);

    mContentLayout->addWidget(outputSettingsLabelWidget);

    mContentLayout->setMargin(0);
    mContentLayout->setSpacing(0);
}

void RenderInstanceWidget::updateFromSettings() {
    const auto renderState = mSettings.getCurrentState();
    bool enabled = renderState != RenderState::paused &&
       renderState != RenderState::rendering;
    setEnabled(enabled);
    QString nameLabelTxt = QString("%1 ").arg(mSettings.getName());
    if(renderState == RenderState::error) {
        nameLabelTxt += tr(": Error"); //mSettings.getRenderError();
    } else if(renderState == RenderState::finished) {
        nameLabelTxt += tr(": Finished");
        mCheckBox->setChecked(false);
    } else if(renderState == RenderState::rendering) {
        nameLabelTxt += tr(": Rendering ...");
    } else if(renderState == RenderState::waiting) {
        nameLabelTxt += tr(": Waiting ...");
    } else if(renderState == RenderState::paused) {
        nameLabelTxt += tr(": Paused");
    }
    mNameLabel->setText(nameLabelTxt);

    QString destinationTxt = mSettings.getOutputDestination();
    mOutputDestinationLineEdit->setText(destinationTxt);
    const OutputSettings &outputSettings = mSettings.getOutputRenderSettings();
    /*OutputSettingsProfile *outputProfile = mSettings.getOutputSettingsProfile();
    QString outputTxt;
    if(outputProfile) {
        outputTxt = outputProfile->getName();
    } else {
        const auto formatT = outputSettings.fOutputFormat;
        if(formatT) {
            outputTxt = tr("Custom: %1").arg(QString(formatT->name));
        } else {
            outputTxt = tr("Output Settings ...");
        }
    }

    mOutputSettingsButton->setText(outputTxt);*/
    mOutputSettingsDisplayWidget->setOutputSettings(outputSettings);

    const RenderSettings &renderSettings = mSettings.getRenderSettings();
    mRenderSettingsDisplayWidget->setRenderSettings(mSettings.getTargetCanvas(),
                                                    renderSettings);
}

RenderInstanceSettings &RenderInstanceWidget::getSettings() {
    return mSettings;
}

void RenderInstanceWidget::mousePressEvent(QMouseEvent *e) {
    if(e->button() == Qt::RightButton) {
        QMenu menu(this);
        menu.addAction("Duplicate");
        const auto state = mSettings.getCurrentState();
        const bool deletable = state != RenderState::rendering &&
                               state != RenderState::paused;
        menu.addAction("Delete")->setEnabled(deletable);
        const auto act = menu.exec(e->globalPos());
        if(act) {
            if(act->text() == "Duplicate") {
                emit duplicate(mSettings);
            } else if(act->text() == "Delete") {
                deleteLater();
            }
        }
    } else return ClosableContainer::mousePressEvent(e);
}

void RenderInstanceWidget::openOutputSettingsDialog() {
    const OutputSettings &outputSettings = mSettings.getOutputRenderSettings();
    const auto dialog = new OutputSettingsDialog(outputSettings, this);
    if(dialog->exec()) {
        mSettings.setOutputSettingsProfile(nullptr);
        OutputSettings outputSettings = dialog->getSettings();
        mSettings.setOutputRenderSettings(outputSettings);
        /*const auto outputFormat = outputSettings.fOutputFormat;
        if(!outputFormat) {
            mOutputSettingsButton->setText("Settings");
        } else {
            mOutputSettingsButton->setText("Custom " +
                        QString(outputFormat->long_name));
        }*/
        mOutputSettingsDisplayWidget->setOutputSettings(outputSettings);
        updateOutputDestinationFromCurrentFormat();
    }
    delete dialog;
}

#include "Private/document.h"

void RenderInstanceWidget::updateOutputDestinationFromCurrentFormat() {
    const OutputSettings &outputSettings = mSettings.getOutputRenderSettings();
    const auto format = outputSettings.fOutputFormat;
    if(!format) return;
    const bool isImgSeq = !std::strcmp(format->name, "image2");
    QString outputDst = mSettings.getOutputDestination();
    if(outputDst.isEmpty()) {
        outputDst = Document::sInstance->projectDirectory() + "/untitled";
    }
    const QString tmpStr = QString(format->extensions);
    const QStringList supportedExt = tmpStr.split(",");
    const QString fileName = outputDst.split("/").last();
    const QStringList dividedName = fileName.split(".");
    QString currExt;
    if(dividedName.count() > 1) {
        QString namePart = dividedName.at(dividedName.count() - 2);
        if(namePart.count() > 0) {
            currExt = dividedName.last();
        }
    }
    if(!supportedExt.contains(currExt) && !supportedExt.isEmpty()) {
        const QString firstSupported = supportedExt.first();
        if(!firstSupported.isEmpty()) {
            if(currExt.isEmpty()) {
                if(outputDst.right(1) == ".") {
                    outputDst = outputDst.left(outputDst.count() - 1);
                }
            } else {
                int extId = outputDst.lastIndexOf("." + currExt);
                outputDst.remove(extId, 1 + currExt.count());
            }
            outputDst += "." + firstSupported;
        }
    }
    if(isImgSeq && outputDst.contains(".") && !outputDst.contains("%05d")) {
        QStringList div = outputDst.split(".");
        div.replace(div.count() - 2, div.at(div.count() - 2) + "%05d");
        outputDst = div.join(".");
    }
    mSettings.setOutputDestination(outputDst);
    mOutputDestinationLineEdit->setText(outputDst);
}

void RenderInstanceWidget::outputSettingsProfileSelected(OutputSettingsProfile *profile) {
    mSettings.setOutputSettingsProfile(profile);
    updateOutputDestinationFromCurrentFormat();
    updateFromSettings();
}

void RenderInstanceWidget::openOutputDestinationDialog() {
    QString supportedExts;
    QString selectedExt;
    const OutputSettings &outputSettings = mSettings.getOutputRenderSettings();
    const auto format = outputSettings.fOutputFormat;
    if(format) {
        QString tmpStr = QString(format->extensions);
        const QStringList supportedExt = tmpStr.split(",");
        selectedExt = "." + supportedExt.first();
        tmpStr.replace(",", " *.");
        supportedExts = "Output File (*." + tmpStr + ")";
    }
    QString iniText = mSettings.getOutputDestination();
    if(iniText.isEmpty()) {
        iniText = Document::sInstance->projectDirectory() +
                  "/untitled" + selectedExt;
    }
    QString saveAs = eDialogs::saveFile("Output Destination",
                                        iniText, supportedExts);
    if(saveAs.isEmpty()) return;
    mSettings.setOutputDestination(saveAs);
    mOutputDestinationLineEdit->setText(saveAs);
    updateOutputDestinationFromCurrentFormat();
}

#include "rendersettingsdialog.h"
void RenderInstanceWidget::openRenderSettingsDialog() {
    const auto dialog = new RenderSettingsDialog(mSettings, this);
    if(dialog->exec()) {
        const RenderSettings sett = dialog->getSettings();
        mSettings.setRenderSettings(sett);
        mSettings.setTargetCanvas(dialog->getCurrentScene());
        updateFromSettings();
    }
    delete dialog;
}

void RenderInstanceWidget::write(eWriteStream &dst) const {
    mSettings.write(dst);
    dst << isChecked();
}

void RenderInstanceWidget::read(eReadStream &src) {
    mSettings.read(src);
    bool checked; src >> checked;
    setChecked(checked);
}

#include "Private/esettings.h"
OutputProfilesListButton::OutputProfilesListButton(RenderInstanceWidget *parent) :
    QPushButton(parent) {
    mParentWidget = parent;
    setIcon(QIcon::fromTheme("dots"));
    setToolTip(tr("Select output profile"));
}

void OutputProfilesListButton::mousePressEvent(QMouseEvent *e) {
    if(e->button() == Qt::LeftButton) {
        QMenu menu;
        int i = 0;
        for(const auto& profile :
            OutputSettingsProfile::sOutputProfiles) {
            QAction *actionT = new QAction(profile->getName());
            actionT->setData(QVariant(i));
            menu.addAction(actionT);
            i++;
        }
        if(OutputSettingsProfile::sOutputProfiles.isEmpty()) {
            menu.addAction(tr("No profiles"))->setEnabled(false);
        }
        menu.addSeparator();
        QAction *actionT = new QAction(tr("Edit..."));
        actionT->setData(QVariant(-1));
        menu.addAction(actionT);

        QAction *selectedAction = menu.exec(mapToGlobal(QPoint(0, height())));
        if(selectedAction) {
            int profileId = selectedAction->data().toInt();
            if(profileId == -1) {
                const OutputSettings &outputSettings =
                        mParentWidget->getSettings().getOutputRenderSettings();
                const auto dialog = new OutputProfilesDialog(outputSettings, this);
                if(dialog->exec()) {
                    const auto profile = dialog->getCurrentProfile();
                    emit profileSelected(profile);
                }
            } else {
                const auto profile = OutputSettingsProfile::
                        sOutputProfiles.at(profileId).get();
                emit profileSelected(profile);
            }
        }
    }
}
