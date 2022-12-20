#! /bin/bash
set -x
set -e

# building in temporary directory to keep system clean
# use RAM disk if possible (as in: not building on CI system like Travis, and RAM disk is available)
if [ "$CI" == "" ] && [ -d /dev/shm ]; then
    TEMP_BASE=/dev/shm
else
    TEMP_BASE=/tmp
fi

BUILD_DIR=$(mktemp -d -p "$TEMP_BASE" appimage-build-XXXXXX)

# make sure to clean up build dir, even if errors occur
cleanup () {
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
    fi
}
trap cleanup EXIT

# store repo root as variable
REPO_ROOT=$(readlink -f $(dirname $(dirname "$0")))
OLD_CWD=$(readlink -f .)

# download appimage packaging tool to old dir
# "$OLD_CWD"/build is in gitignore. Built files are stored at $BUILD_DIR instead
mkdir -p "$OLD_CWD"/bin "$OLD_CWD"/build
pushd "$OLD_CWD"/build
wget --timeout=5 -N \
    https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
wget --timeout=5 -N \
    https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
chmod +x linuxdeploy*.AppImage
popd

# switch to build dir
pushd "$BUILD_DIR"

# configure gui build files with CMake
cmake -DCMAKE_BUILD_TYPE:STRING=Release \
    "$@" -DCMAKE_INSTALL_PREFIX=/usr "${REPO_ROOT}"
    
# build gui and install files into AppDir
make "-j$(nproc)"
make install DESTDIR=AppDir
# Not needed for seev executable
rm AppDir/usr/bin/orient AppDir/usr/lib/liborie.a
rm -r AppDir/usr/include

# initialize AppDir, bundle shared libraries, use Qt plugin to bundle additional resources, and build AppImage, all in one single command
"$OLD_CWD"/build/linuxdeploy-x86_64.AppImage --appdir AppDir --plugin qt --output appimage

# move built AppImage back into original CWD
mv ./*.AppImage "$OLD_CWD"/bin/SearchEverywhere.AppImage
