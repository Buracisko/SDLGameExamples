@echo off
mkdir build
cmake -S . -B ./build ^
-DSDL2_PATH="libraries/SDL2-2.26.3" ^
-DSDL2_IMAGE_PATH="libraries/SDL2_image-2.6.3" ^
-DSDL2_MIXER_PATH="libraries/SDL2_mixer-2.6.3" ^
-DSDL2_TTF_PATH="libraries/SDL2_ttf-2.20.2"

echo Creating a asset junction
set assetJunction="build/assets"
@rem I would like to check if the assetJunction is a junction, but effort is too high
@rem https://stackoverflow.com/questions/18883892/batch-file-windows-cmd-exe-test-if-a-directory-is-a-link-symlink
if not exist %assetJunction% mklink /j %assetJunction% "./assets"
echo Solution ready
