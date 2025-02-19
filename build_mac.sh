#!/usr/bin/env bash
# prerequisite: brew install qt@5 freeglut mesa harfbuzz

sys_glut_clfags=`pkg-config --cflags glut gl`
sys_glut_libs=`pkg-config --libs glut gl`
sys_harfbuzz_clfags=`pkg-config --cflags harfbuzz`
sys_harfbuzz_libs=`pkg-config --libs harfbuzz`
cd mupdf
make USE_SYSTEM_HARFBUZZ=yes USE_SYSTEM_GLUT=yes SYS_GLUT_CFLAGS="${sys_glut_clfags}" SYS_GLUT_LIBS="${sys_glut_libs}" SYS_HARFBUZZ_CFLAGS="${sys_harfbuzz_clfags}" SYS_HARFBUZZ_LIBS="${sys_harfbuzz_libs}" -j 4
cd ..
qmake pdf_viewer_build_config.pro
make

rm -r build 2> /dev/null
mkdir build
mv sioyek.app build/
cp -r pdf_viewer/shaders build/sioyek.app/Contents/MacOS/shaders

cp pdf_viewer/prefs.config build/sioyek.app/Contents/MacOS/prefs.config
cp pdf_viewer/prefs_user.config build/sioyek.app/Contents/MacOS/prefs_user.config
cp pdf_viewer/keys.config build/sioyek.app/Contents/MacOS/keys.config
cp pdf_viewer/keys_user.config build/sioyek.app/Contents/MacOS/keys_user.config
cp tutorial.pdf build/sioyek.app/Contents/MacOS/tutorial.pdf
