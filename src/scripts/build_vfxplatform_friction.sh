#!/bin/bash
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

set -e -x

source /opt/rh/llvm-toolset-7.0/enable
clang -v

CWD=${CWD:-`pwd`}
SDK=${SDK:-"/opt/friction"}
BUILD=${BUILD:-"${HOME}"}

REL=${REL:-1}
BRANCH=${BRANCH:-""}
COMMIT=${COMMIT:-""}
TAG=${TAG:-""}

export PATH="${SDK}/bin:${PATH}"
export PKG_CONFIG_PATH="${SDK}/lib/pkgconfig"
export LD_LIBRARY_PATH="${SDK}/lib:${LD_LIBRARY_PATH}"

if [ ! -d "${SDK}" ]; then
    echo "MISSING SDK"
    exit 1
fi

if [ ! -d "${BUILD}" ]; then
    mkdir -p ${BUILD}
fi

CHECKOUT="main"
if [ "${BRANCH}" != "" ]; then
    CHECKOUT="${BRANCH}"
elif [ "${COMMIT}" != "" ]; then
    CHECKOUT="${COMMIT}"
elif [ "${TAG}" != "" ]; then
    CHECKOUT="tags/${TAG}"
fi

if [ ! -d "${BUILD}/friction" ]; then
    (cd ${BUILD} ;
        git clone https://github.com/friction2d/friction
        cd friction
        git checkout ${CHECKOUT}
        git submodule update -i --recursive docs
    )
fi

cd ${BUILD}/friction

if [ ! -d "src/skia/out" ]; then
    mv src/skia src/skia.orig
    ln -sf ${SDK}/skia src/skia
fi

if [ ! -d "src/gperftools/.libs" ]; then
    mv src/gperftools src/gperftools.orig
    ln -sf ${SDK}/gperftools src/gperftools
fi

rm -rf build-vfxplatform || true
mkdir build-vfxplatform && cd build-vfxplatform

REL_STATUS="ON"
if [ "${REL}" != 1 ]; then
    REL_STATUS="OFF"
fi

cmake -GNinja \
-DCMAKE_INSTALL_PREFIX=${SDK} \
-DCMAKE_PREFIX_PATH=${SDK} \
-DCMAKE_BUILD_TYPE=Release \
-DLINUX_DEPLOY=ON \
-DFRICTION_OFFICIAL_RELEASE=${REL_STATUS} \
-DQSCINTILLA_INCLUDE_DIRS=${SDK}/include \
-DQSCINTILLA_LIBRARIES_DIRS=${SDK}/lib \
-DQSCINTILLA_LIBRARIES=qscintilla2_friction_qt5 \
-DCMAKE_CXX_COMPILER=clang++ \
-DCMAKE_C_COMPILER=clang \
..

VERSION=`cat version.txt`
if [ "${REL}" != 1 ]; then
    GIT=`git rev-parse --short HEAD`
    VERSION="${VERSION}-dev-${GIT}"
fi

cmake --build .

FRICTION_INSTALL_DIR=friction-${VERSION}
mkdir -p ${BUILD}/${FRICTION_INSTALL_DIR}/opt/friction/{bin,lib,share} || true
mkdir -p ${BUILD}/${FRICTION_INSTALL_DIR}/opt/friction/plugins/{audio,generic,platforminputcontexts,platforms,xcbglintegrations} || true
DESTDIR=${BUILD}/${FRICTION_INSTALL_DIR} cmake --build . --target install
