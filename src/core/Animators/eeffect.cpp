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

#include "eeffect.h"

eEffect::eEffect(const QString &name) :
    StaticComplexAnimator(name) {
    ca_setDisabledWhenEmpty(false);
}

void eEffect::prp_writeProperty_impl(eWriteStream& dst) const {
    StaticComplexAnimator::prp_writeProperty_impl(dst);
    dst << mVisible;
}

void eEffect::prp_readProperty_impl(eReadStream& src) {
    StaticComplexAnimator::prp_readProperty_impl(src);
    bool visible; src >> visible;
    setVisible(visible);
}

void eEffect::switchVisible() {
    setVisible(!mVisible);
}

void eEffect::setVisible(const bool visible) {
    if(visible == mVisible) return;
    {
        prp_pushUndoRedoName(visible ? "Hide Effect" : "Show Effect");
        UndoRedo ur;
        const auto oldValue = mVisible;
        const auto newValue = visible;
        ur.fUndo = [this, oldValue]() {
            setVisible(oldValue);
        };
        ur.fRedo = [this, newValue]() {
            setVisible(newValue);
        };
        prp_addUndoRedo(ur);
    }
    mVisible = visible;
    prp_afterWholeInfluenceRangeChanged();
    emit effectVisibilityChanged(visible);
}

bool eEffect::isVisible() const {
    return mVisible;
}
