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

#ifndef BOXSINGLEWIDGET_H
#define BOXSINGLEWIDGET_H

#include "OptimalScrollArea/singlewidget.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMimeData>
#include <QComboBox>
#include "skia/skiaincludes.h"
#include "smartPointers/ememory.h"
#include "framerange.h"
#include "Animators/SmartPath/smartpathanimator.h"
class QrealAnimatorValueSlider;
class TimelineMovable;
class Key;
class BoxTargetWidget;
class BoxesListActionButton;
class PixmapActionButton;
class BoolPropertyWidget;
class ComboBoxProperty;
class ColorAnimator;
class ColorAnimatorButton;
class BoxScroller;
class eComboBox;

class BoxSingleWidget : public SingleWidget {
public:
    explicit BoxSingleWidget(BoxScroller * const parent);

    void setTargetAbstraction(SWT_Abstraction *abs);

    static QPixmap* VISIBLE_PIXMAP;
    static QPixmap* INVISIBLE_PIXMAP;
    static QPixmap* BOX_CHILDREN_VISIBLE;
    static QPixmap* BOX_CHILDREN_HIDDEN;
    static QPixmap* ANIMATOR_CHILDREN_VISIBLE;
    static QPixmap* ANIMATOR_CHILDREN_HIDDEN;
    static QPixmap* LOCKED_PIXMAP;
    static QPixmap* UNLOCKED_PIXMAP;
    static QPixmap* MUTED_PIXMAP;
    static QPixmap* UNMUTED_PIXMAP;
    static QPixmap* ANIMATOR_RECORDING;
    static QPixmap* ANIMATOR_NOT_RECORDING;
    static QPixmap* ANIMATOR_DESCENDANT_RECORDING;
    static QPixmap* C_PIXMAP;
    static QPixmap* G_PIXMAP;
    static QPixmap* CG_PIXMAP;
    static QPixmap* GRAPH_PROPERTY;
    static QPixmap* PROMOTE_TO_LAYER_PIXMAP;

    static bool sStaticPixmapsLoaded;
    static void loadStaticPixmaps();
    static void clearStaticPixmaps();

    void prp_drawTimelineControls(QPainter * const p,
                  const qreal pixelsPerFrame,
                  const FrameRange &viewedFrames);
    Key *getKeyAtPos(const int pressX,
                     const qreal pixelsPerFrame,
                     const int minViewedFrame);
    void getKeysInRect(const QRectF &selectionRect,
                       const qreal pixelsPerFrame,
                       QList<Key *> &listKeys);
    TimelineMovable *getRectangleMovableAtPos(
                        const int pressX,
                        const qreal pixelsPerFrame,
                        const int minViewedFrame);

    void setSelected(const bool selected) {
        mSelected = selected;
        update();
    }
protected:
    bool mSelected = false;
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void resizeEvent(QResizeEvent *);

    bool mBlendModeVisible = false;
    bool mPathBlendModeVisible = false;
    bool mFillTypeVisible = false;

    void updatePathCompositionBoxVisible();
    void updateCompositionBoxVisible();
    void updateFillTypeBoxVisible();

    void clearAndHideValueAnimators();
    void updateValueSlidersForQPointFAnimator();
private:
    ContainerBox *getPromoteTargetGroup();

    void clearSelected() { setSelected(false); }
    void switchContentVisibleAction();
    void switchRecordingAction();
    void switchBoxLockedAction();

    void switchBoxVisibleAction();
    void setCompositionMode(const int id);
    void setPathCompositionMode(const int id);
    void setFillType(const int id);
    ColorAnimator* getColorTarget() const;

    void setComboProperty(ComboBoxProperty * const combo);

    BoxScroller* const mParent;

    bool mDragPressPos = false;
    QPoint mDragStartPos;

    PixmapActionButton *mRecordButton;
    PixmapActionButton *mContentButton;
    PixmapActionButton *mVisibleButton;
    PixmapActionButton *mLockedButton;
    PixmapActionButton *mHwSupportButton;
    ColorAnimatorButton *mColorButton;
    BoxTargetWidget *mBoxTargetWidget;

    QWidget *mFillWidget;
    BoolPropertyWidget *mCheckBox;
    QHBoxLayout *mMainLayout;
    QrealAnimatorValueSlider *mValueSlider;
    QrealAnimatorValueSlider *mSecondValueSlider;

    PixmapActionButton *mPromoteToLayerButton;
    eComboBox *mPropertyComboBox;
    eComboBox *mBlendModeCombo;
    eComboBox *mPathBlendModeCombo;
    eComboBox *mFillTypeCombo;

    ConnContext mTargetConn;
};

#endif // BOXSINGLEWIDGET_H
