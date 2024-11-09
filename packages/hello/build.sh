preset=gnu
name=hello
version=2.12.1
description="GNU Friendly greeting program"
checksum=8d99142afd92576f30b0cd7cb42a8dc6809998bc5d607d88761f512e26c7db20

run_preset

if [[ $(uname) == "Darwin" ]]; then
    stage50="make CFLAGS=\"-liconv\""
fi