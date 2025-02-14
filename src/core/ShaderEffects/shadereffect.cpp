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

#include "shadereffect.h"
#include "RasterEffects/rastereffectcollection.h"
#include "shadereffectcaller.h"

ShaderEffect::ShaderEffect(const QString& name,
                           const ShaderEffectCreator * const creator,
                           const ShaderEffectProgram * const program,
                           const QList<stdsptr<ShaderPropertyCreator>> &props) :
    RasterEffect(name, HardwareSupport::gpuOnly, false,
                 RasterEffectType::CUSTOM_SHADER),
    mProgram(program), mCreator(creator) {
    for(const auto& propC : props)
        ca_addChild(propC->create());
}

void ShaderEffect::writeIdentifier(eWriteStream& dst) const {
    RasterEffect::writeIdentifier(dst);
    mCreator->writeIdentifier(dst);
}

void ShaderEffect::writeIdentifierXEV(QDomElement& ele) const {
    RasterEffect::writeIdentifierXEV(ele);
    mCreator->writeIdentifierXEV(ele);
}

stdsptr<RasterEffectCaller> ShaderEffect::getEffectCaller(
        const qreal relFrame, const qreal resolution,
        const qreal influence, BoxRenderData * const data) const {
    Q_UNUSED(data)
    std::unique_ptr<ShaderEffectJS> engineUPtr;
    takeJSEngine(engineUPtr);
    ShaderEffectJS& engine = *engineUPtr;
    const auto effect = enve::make_shared<ShaderEffectCaller>(
                            std::move(engineUPtr), *mProgram);

    QJSValueList setterArgs;
    UniformSpecifiers& uniSpecs = effect->mUniformSpecifiers;
    const int argsCount = mProgram->fPropUniLocs.count();
    for(int i = 0; i < argsCount; i++) {
        const GLint loc = mProgram->fPropUniLocs.at(i);
        const auto prop = ca_getChildAt(i);
        const auto& uniformC = mProgram->fPropUniCreators.at(i);
        uniformC->create(engine, loc, prop, relFrame,
                         resolution, influence,
                         setterArgs, uniSpecs);
    }
    engine.setValues(setterArgs);
    const int valsCount = mProgram->fValueHandlers.count();
    for(int i = 0; i < valsCount; i++) {
        const GLint loc = mProgram->fValueLocs.at(i);
        const auto& value = mProgram->fValueHandlers.at(i);
        uniSpecs << value->create(loc, &engine.getGlValueGetter(i));
    }
    return effect;
}

void ShaderEffect::giveBackJSEngine(stduptr<ShaderEffectJS>&& engineUPtr) {
    mProgram->fEngines.push_back(std::move(engineUPtr));
}

void ShaderEffect::takeJSEngine(stduptr<ShaderEffectJS>& engineUPtr) const {
    if(mProgram->fEngines.empty()) {
        engineUPtr = std::make_unique<ShaderEffectJS>(*mProgram->fJSBlueprint);
    } else {
        engineUPtr = std::move(mProgram->fEngines.back());
        mProgram->fEngines.pop_back();
    }
}
