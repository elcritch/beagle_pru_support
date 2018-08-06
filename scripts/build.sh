#!/usr/bin/env bash

# Example
#
# archive.sh /path/to/build_dir /path/to/artifact.tar.xz


set -e
set -x

WORK_DIR=$1
# TARBALL_PATH=$2
INSTALL_DIR=${2:-_build/pru/}

BUILD_OS=$(uname -s)

if [[ $BUILD_OS = "Darwin" ]]; then
    # Use GNU tar from Homebrew (brew install gnu-tar)
    TAR=gtar
else
    TAR=tar
fi


# Assemble the tarball for the toolchain
# TARGET_TUPLE=$INSTALL_DIR
# TAR_PATH="${TARBALL_PATH}"
# TOOLCHAIN_BASE_NAME="$(basename ${TARBALL_PATH%.*.*})/"

MIX_ENV=test PRU_CGT=/dev/null make clean
make artifact

# Copy build files
mkdir -p $WORK_DIR/$INSTALL_DIR
cp -R $INSTALL_DIR/ $WORK_DIR/$INSTALL_DIR


