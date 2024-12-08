# FFmpeg 构建

## Windows
在`FFmpegLib`目录下执行`.\win-ffmpeg-static-download.ps1`
> [https://github.com/icuxika/FFmpegWindowsBuild](https://github.com/icuxika/FFmpegWindowsBuild) 根据此项目构建出Release和Debug版本的静态`FFmpeg`库

## `Ubuntu 22.04.5 LTS`
```
git clone https://code.videolan.org/videolan/x264.git
cd x264/
sudo apt install nasm
./configure --prefix=/home/icuxika/CommandLineTools/x264 --enable-static
make -j8
make install
```
```
wget -e "https_proxy=http://192.168.50.88:7890"  https://ffmpeg.org/releases/ffmpeg-6.1.1.tar.xz
xz -d ffmpeg-6.1.1.tar.xz
tar -xvf ffmpeg-6.1.1.tar -C .
export PKG_CONFIG_PATH=/home/icuxika/CommandLineTools/x264/lib/pkgconfig/
./configure --prefix=/home/icuxika/CommandLineTools/ffmpeg-6.1.1-static --enable-gpl --enable-libx264 --disable-everything --enable-encoder=libx264 --enable-decoder=h264 --enable-muxer=mp4 --enable-demuxer=mov --enable-parser=h264 --enable-protocol=file --enable-filter=scale --enable-filter=crop --pkg-config-flags=--static
make -j16
make install
```