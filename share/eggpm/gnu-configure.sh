run_preset () {
    stage00="cd $name-$version"
    stage20="./configure --prefix=/usr/local"
    stage50="make"
    stage80="make install DESTDIR=\$(pwd)/../build"
    stage98="cd .."
    stage99="rm -rf $name-$version"
}