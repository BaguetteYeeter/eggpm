GNU_URL=https://ftp.gnu.org

run_preset () {
    url=$GNU_URL/gnu/$name/$name-$version.tar.gz
    stage00="cd $name-$version"
    stage20="./configure --prefix=/usr"
    stage50="make"
    stage80="make install DESTDIR=\$(pwd)/../build"
    stage98="cd .."
    stage99="rm -rf $name-$version"
}