name=libiconv
version=1.17
url=https://ftp.gnu.org/gnu/$name/$name-$version.tar.gz
checksum=8f74213b56238c85a50a5329f77e06198771e70dd9a739779f4c02f65d971313

stage00="cd $name-$version"
stage20="./configure --prefix=/usr/local"
stage50="make"
stage80="make install DESTDIR=\$(pwd)/../build"
stage98="cd .."
stage99="rm -rf $name-$version"