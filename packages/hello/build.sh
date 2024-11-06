name=hello
version=2.12.1
url=https://ftp.gnu.org/gnu/$name/$name-$version.tar.gz
checksum=8d99142afd92576f30b0cd7cb42a8dc6809998bc5d607d88761f512e26c7db20

stage00="cd $name-$version"
stage20="./configure --prefix=/usr/local"
if [[ $(uname) == "Darwin" ]]; then
    stage50="make LDFLAGS=\"-L/usr/local/opt/libiconv/lib\" CPPFLAGS=\"-I/usr/local/opt/libiconv/include\" CFLAGS=\"-liconv\""
else
    stage50="make"
fi
stage80="make install DESTDIR=../build"
stage98="cd .."
stage99="rm -rf $name-$version"