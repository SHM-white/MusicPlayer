export QMAKE=$1

mkdir AppDir
mkdir AppDir/usr
mkdir AppDir/usr/bin
mkdir AppDir/usr/lib
mkdir AppDir/usr/share
mkdir AppDir/usr/share/applications

if(exist)
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/1-alpha-20250213-1/linuxdeploy-plugin-qt-x86_64.AppImage
mv linuxdeploy-x86_64.AppImage linuxdeploy
sudo chmod +x ./linuxdeploy
sudo chmod +x ./linuxdeploy-plugin-qt-x86_64.AppImage
cp build/MusicPlayer AppDir/usr/bin/
./linuxdeploy --appdir AppDir --executable AppDir/usr/bin/MusicPlayer --desktop-file AppDir/usr/share/applications/MusicPlayer.desktop --output appimage --library /usr/lib/x86_64-linux-gnu/libstdc++.so.6 --plugin qt