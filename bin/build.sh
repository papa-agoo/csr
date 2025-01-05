test -d build || ./bin/cmake.sh

(cd build && make all install)
