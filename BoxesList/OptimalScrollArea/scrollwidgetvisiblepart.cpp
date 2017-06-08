#include "scrollwidgetvisiblepart.h"
#include <QPainter>
#include "singlewidgetabstraction.h"
#include "singlewidget.h"
#include "scrollwidget.h"
#include "singlewidgettarget.h"
#include "mainwindow.h"
#include "global.h"

QList<ScrollWidgetVisiblePart*> ScrollWidgetVisiblePart::mAllInstances;

ScrollWidgetVisiblePart::ScrollWidgetVisiblePart(
        ScrollWidget *parent) :
    QWidget(parent) {
    mCurrentRulesCollection.rule = SWT_NoRule;
    mParentWidget = parent;
    addInstance(this);
}

ScrollWidgetVisiblePart::~ScrollWidgetVisiblePart() {
    removeInstance(this);
    if(mMainAbstraction != NULL) {
        mMainAbstraction->deleteWithDescendantAbstraction();
    }
}

void ScrollWidgetVisiblePart::setVisibleTop(const int &top) {
    mVisibleTop = top;
    updateVisibleWidgetsContent();
}

void ScrollWidgetVisiblePart::setVisibleHeight(const int &height) {
    setFixedHeight(height);
    mVisibleHeight = height;
    updateVisibleWidgets();
}

void ScrollWidgetVisiblePart::updateWidgetsWidth() {
    Q_FOREACH(SingleWidget *widget, mSingleWidgets) {
        widget->setFixedWidth(width() - widget->x());
    }
}

void ScrollWidgetVisiblePart::callUpdaters() {
    if(mVisibleWidgetsContentUpdateScheduled ||
       mParentHeightUpdateScheduled) {
        updateParentHeightIfNeeded();
        updateVisibleWidgetsContentIfNeeded();
    }
    update();
}

void ScrollWidgetVisiblePart::callAllInstanceUpdaters() {
    Q_FOREACH(ScrollWidgetVisiblePart *instance, mAllInstances) {
        instance->callUpdaters();
    }
}

void ScrollWidgetVisiblePart::addInstance(
        ScrollWidgetVisiblePart *instance) {
    mAllInstances.append(instance);
}

void ScrollWidgetVisiblePart::removeInstance(
        ScrollWidgetVisiblePart *instance) {
    mAllInstances.removeOne(instance);
}

void ScrollWidgetVisiblePart::setCurrentRule(
        const SWT_Rule &rule) {
    mCurrentRulesCollection.rule = rule;
    updateParentHeight();
    updateVisibleWidgetsContent();
}

void ScrollWidgetVisiblePart::setCurrentTarget(
        SingleWidgetTarget *targetP,
        const SWT_Target &target) {
    mCurrentRulesCollection.target = target;
    mParentWidget->setMainTarget(targetP);
    updateParentHeight();
    updateVisibleWidgetsContent();
}

void ScrollWidgetVisiblePart::setCurrentType(SWT_Checker type) {
    mCurrentRulesCollection.type = type;
    updateParentHeight();
    updateVisibleWidgetsContent();
}

void ScrollWidgetVisiblePart::setAlwaysShowChildren(
        const bool &alwaysShowChildren) {
    mCurrentRulesCollection.alwaysShowChildren = alwaysShowChildren;
    updateParentHeight();
    updateVisibleWidgetsContent();
}

void ScrollWidgetVisiblePart::setCurrentSearchText(
        const QString &text) {
    mCurrentRulesCollection.searchString = text;
    updateParentHeight();
    updateVisibleWidgetsContent();
}

void ScrollWidgetVisiblePart::scheduleContentUpdateIfIsCurrentRule(
        const SWT_Rule &rule) {
    if(isCurrentRule(rule)) {
        scheduleUpdateParentHeight();
        scheduledUpdateVisibleWidgetsContent();
    }
}

void ScrollWidgetVisiblePart::scheduleContentUpdateIfIsCurrentTarget(
        SingleWidgetTarget *targetP,
        const SWT_Target &target) {
    if(mCurrentRulesCollection.target == target) {
        mParentWidget->setMainTarget(targetP);
        scheduleUpdateParentHeight();
        scheduledUpdateVisibleWidgetsContent();
    }
}

void ScrollWidgetVisiblePart::scheduleContentUpdateIfSearchNotEmpty() {
    if(mCurrentRulesCollection.searchString.isEmpty()) return;
    scheduleUpdateParentHeight();
    scheduledUpdateVisibleWidgetsContent();
}

bool ScrollWidgetVisiblePart::isCurrentRule(const SWT_Rule &rule) {
    return rule == mCurrentRulesCollection.rule;
}

void ScrollWidgetVisiblePart::scheduledUpdateVisibleWidgetsContent() {
    mVisibleWidgetsContentUpdateScheduled = true;
}

void ScrollWidgetVisiblePart::updateVisibleWidgetsContentIfNeeded() {
    if(mVisibleWidgetsContentUpdateScheduled) {
        mVisibleWidgetsContentUpdateScheduled = false;
        updateVisibleWidgetsContent();
    }
}

void ScrollWidgetVisiblePart::scheduleUpdateParentHeight() {
    mParentHeightUpdateScheduled = true;
}

void ScrollWidgetVisiblePart::updateParentHeightIfNeeded() {
    if(mParentHeightUpdateScheduled) {
        mParentHeightUpdateScheduled = false;
        updateParentHeight();
    }
}

void ScrollWidgetVisiblePart::updateVisibleWidgets() {
    int neededWidgets = qCeil(mVisibleHeight/
                              (qreal)MIN_WIDGET_HEIGHT);
    int currentNWidgets = mSingleWidgets.count();

    if(neededWidgets == currentNWidgets) return;
    if(neededWidgets > currentNWidgets) {
        for(int i = neededWidgets - currentNWidgets; i > 0; i--) {
            SingleWidget *newWidget = createNewSingleWidget();
            mSingleWidgets.append(newWidget);
        }
    } else {
        for(int i = currentNWidgets - neededWidgets; i > 0; i--) {
            delete mSingleWidgets.takeLast();
        }
    }

    int yT = 0;
    Q_FOREACH(SingleWidget *widget, mSingleWidgets) {
        widget->move(widget->x(), yT);
        widget->setFixedWidth(width() - widget->x());
        yT += MIN_WIDGET_HEIGHT;
    }

    updateVisibleWidgetsContent();
}

void ScrollWidgetVisiblePart::updateVisibleWidgetsContent() {
    if(mMainAbstraction == NULL) return;
    //updateParentHeight();
    int idP = 0;
    int currX;
    int currY;
//    if(mSkipMainAbstraction) {
//        currX = -20;
//        currY = -10;
//    } else {
        currX = 0;
        currY = MIN_WIDGET_HEIGHT/2;
//    }
    mMainAbstraction->setSingleWidgetAbstractions(
                mVisibleTop,
                mVisibleTop + mVisibleHeight + currY,
                &currY, currX,
                &mSingleWidgets,
                &idP,
                mCurrentRulesCollection,
                true,
                false);

    for(int i = idP; i < mSingleWidgets.count(); i++) {
        mSingleWidgets.at(i)->hide();
    }
}

void ScrollWidgetVisiblePart::setMainAbstraction(
        SingleWidgetAbstraction *abs) {
    mMainAbstraction = abs;
    scheduledUpdateVisibleWidgetsContent();
//    if(abs == NULL) return;
//    abs->setContentVisible(true);
//    updateVisibleWidgetsContent();
//    updateParentHeight();
}

void ScrollWidgetVisiblePart::updateParentHeight() {
    mParentWidget->updateHeight();
}

SingleWidget *ScrollWidgetVisiblePart::createNewSingleWidget() {
    return new SingleWidget(this);
}

SWT_RulesCollection::SWT_RulesCollection() {
    type = NULL;
    rule = SWT_NoRule;
    alwaysShowChildren = false;
    target = SWT_CurrentCanvas;
    searchString = "";
}
