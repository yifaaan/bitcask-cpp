
CMAKE_CXX_OPT=""

# crc32c
GCRC32_URL='https://github.com/google/crc32c.git'
GCRC32_PATH='crc32c'
GCRC32_BUILD_OPT="${CMAKE_CXX_OPT} -DCRC32C_BUILD_TESTS=0 -DCRC32C_BUILD_BENCHMARKS=0 -DCRC32C_USE_GLOG=0"

# google test
GTEST_URL='https://github.com/google/googletest.git'
GTEST_PATH='googletest'
GTEST_BUILD_OPT=''

# 有错误立刻退出
set -e

source dependency.sh

function init_deps()
{
    mkdir -p third_party && cd third_party

    echo "make_dep ${GCRC32_PATH} ${GCRC32_BUILD_OPT}"
    make_dep ${GCRC32_URL} ${GCRC32_PATH} ${GCRC32_BUILD_OPT}

    echo "make_dep ${GTEST_PATH} ${GTEST_BUILD_OPT}"
    make_dep ${GTEST_URL} ${GTEST_PATH} ${GTEST_BUILD_OPT}
}

function clear()
{
    echo "clean..."
    mkdir -p third_party && cd third_party
    clear_build_history
}

case $1 in
    clear)
        clear
        ;;
    *)
        init_deps
        ;;
esac