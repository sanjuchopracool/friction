// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "Boxes/rectangle.h"
#include "canvas.h"
#include "Animators/gradientpoints.h"
#include "MovablePoints/animatedpoint.h"
#include "Animators/transformanimator.h"
#include "RasterEffects/rastereffectcollection.h"

Rectangle::Rectangle() : PathBox("Rectangle", eBoxType::rectangle) {
    setPointsHandler(enve::make_shared<PointsHandler>());

    mTopLeftAnimator = enve::make_shared<QPointFAnimator>("top left");
    mTopLeftPoint = enve::make_shared<AnimatedPoint>(
                mTopLeftAnimator.get(), mTransformAnimator.data(),
                TYPE_PATH_POINT);
    getPointsHandler()->appendPt(mTopLeftPoint);
    mTopLeftPoint->setRelativePos(QPointF(0, 0));

    mBottomRightAnimator = enve::make_shared<QPointFAnimator>("bottom right");
    mBottomRightPoint = enve::make_shared<AnimatedPoint>(
                mBottomRightAnimator.get(), mTransformAnimator.data(),
                TYPE_PATH_POINT);
    getPointsHandler()->appendPt(mBottomRightPoint);
    mBottomRightPoint->setRelativePos(QPointF(10, 10));

    //mTopLeftPoint->setBottomRightPoint(mBottomRightPoint);
    //mBottomRightPoint->setRadiusPoint(mRadiusPoint);

    ca_addChild(mTopLeftAnimator);
    ca_addChild(mBottomRightAnimator);

    ca_prependChild(mTopLeftAnimator.get(), mRasterEffectsAnimators);
    ca_prependChild(mBottomRightAnimator.get(), mRasterEffectsAnimators);

    mRadiusAnimator = enve::make_shared<QPointFAnimator>("round radius");
    mRadiusAnimator->setValuesRange(0, 9999);

    mRadiusPoint = enve::make_shared<AnimatedPoint>(
                mRadiusAnimator.get(), mTransformAnimator.data(),
                TYPE_PATH_POINT);

    ca_addChild(mRadiusAnimator);
    ca_prependChild(mRadiusAnimator.get(), mRasterEffectsAnimators);

    const auto pathUpdater = [this](const UpdateReason reason) {
        setPathsOutdated(reason);
    };
    connect(mTopLeftAnimator.get(), &Property::prp_currentFrameChanged,
            this, pathUpdater);
    connect(mBottomRightAnimator.get(), &Property::prp_currentFrameChanged,
            this, pathUpdater);
    connect(mRadiusAnimator.get(), &Property::prp_currentFrameChanged,
            this, pathUpdater);
}

SkPath Rectangle::getRelativePath(const qreal relFrame) const {
    SkPath path;
    const QPointF topLeft = mTopLeftAnimator->getEffectiveValue(relFrame);
    const QPointF bottomRight = mBottomRightAnimator->getEffectiveValue(relFrame);
    const QPointF radius = mRadiusAnimator->getEffectiveValue(relFrame);
    const SkRect rect = SkRect::MakeLTRB(topLeft.x(), topLeft.y(),
                                         bottomRight.x(), bottomRight.y());
    path.addRoundRect(rect, radius.x(), radius.y());
    return path;
}

void Rectangle::setTopLeftPos(const QPointF &pos) {
    mTopLeftPoint->setRelativePos(pos);
}

void Rectangle::setBottomRightPos(const QPointF &pos) {
    mBottomRightPoint->setRelativePos(pos);
}

void Rectangle::setYRadius(const qreal radiusY) {
    mRadiusAnimator->getYAnimator()->setCurrentBaseValue(radiusY);
}

void Rectangle::setXRadius(const qreal radiusX) {
    mRadiusAnimator->getXAnimator()->setCurrentBaseValue(radiusX);
}

void Rectangle::moveSizePointByAbs(const QPointF &absTrans) {
    mBottomRightPoint->moveByAbs(absTrans);
}

MovablePoint *Rectangle::getBottomRightPoint() {
    return mBottomRightPoint.get();
}

void Rectangle::getMotionBlurProperties(QList<Property*> &list) const {
    PathBox::getMotionBlurProperties(list);
    list.append(mTopLeftAnimator.get());
    list.append(mBottomRightAnimator.get());
    list.append(mRadiusAnimator.get());
}

bool Rectangle::differenceInEditPathBetweenFrames(
        const int frame1, const int frame2) const {
    if(mTopLeftAnimator->prp_differencesBetweenRelFrames(frame1, frame2)) return true;
    if(mBottomRightAnimator->prp_differencesBetweenRelFrames(frame1, frame2)) return true;
    return mRadiusAnimator->prp_differencesBetweenRelFrames(frame1, frame2);
}

#include "simpletask.h"
#include "Animators/customproperties.h"
#include "Expressions/expression.h"

QDomElement Rectangle::saveSVG(QDomDocument& doc,
                               QDomElement& defs,
                               const FrameRange& absRange,
                               const qreal fps, const bool loop) const {
    const auto copy = enve::make_shared<Rectangle>();
    BoxClipboard::sCopyAndPaste(this, copy.get());
    getParentGroup()->addContained(copy);
    SimpleTask::sProcessAll();

    auto& cProps = copy->mCustomProperties;

    const auto cX = cProps->addPropertyOfType<QrealAnimator>("x");
    const auto cY = cProps->addPropertyOfType<QrealAnimator>("y");
    const auto cW = cProps->addPropertyOfType<QrealAnimator>("width");
    const auto cH = cProps->addPropertyOfType<QrealAnimator>("height");

    const QString bindings = "left = top left.x;"
                             "top = top left.y;"
                             "right = bottom right.x;"
                             "bottom = bottom right.y;";

    const auto xScript = "return Math.min(left, right);";
    const auto yScript = "return Math.min(top, bottom);";
    const auto wScript = "return Math.abs(left - right);";
    const auto hScript = "return Math.abs(top - bottom);";

    const auto xExpr = Expression::sCreate(bindings, "", xScript, cX,
                                           Expression::sQrealAnimatorTester);
    const auto yExpr = Expression::sCreate(bindings, "", yScript, cY,
                                           Expression::sQrealAnimatorTester);
    const auto wExpr = Expression::sCreate(bindings, "", wScript, cW,
                                           Expression::sQrealAnimatorTester);
    const auto hExpr = Expression::sCreate(bindings, "", hScript, cH,
                                           Expression::sQrealAnimatorTester);

    cX->setExpression(xExpr);
    cY->setExpression(yExpr);
    cW->setExpression(wExpr);
    cH->setExpression(hExpr);

    auto ele = doc.createElement("rect");
    const auto xAnim = copy->mTopLeftAnimator->getXAnimator();
    const auto yAnim = copy->mTopLeftAnimator->getYAnimator();

    const auto rightAnim = copy->mBottomRightAnimator->getXAnimator();
    const auto bottomAnim = copy->mBottomRightAnimator->getYAnimator();

    xAnim->anim_coordinateKeysWith(rightAnim);
    yAnim->anim_coordinateKeysWith(bottomAnim);

    cX->saveQrealSVG(doc, ele, defs, absRange, fps, "x", loop);
    cY->saveQrealSVG(doc, ele, defs, absRange, fps, "y", loop);
    cW->saveQrealSVG(doc, ele, defs, absRange, fps, "width", loop);
    cH->saveQrealSVG(doc, ele, defs, absRange, fps, "height", loop);

    savePathBoxSVG(doc, ele, defs, absRange, fps, loop);

    copy->removeFromParent_k();
    return ele;
}
