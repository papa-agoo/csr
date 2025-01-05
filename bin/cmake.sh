BUILD_TYPE=${1:-Release}

test -d build && rm -rf build

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -B build
