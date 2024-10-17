#!bin/bash

function clone_or_update_from_git()
{
    # 第一个参数:git地址
    GIT_REPO_URL=$1
    # 第二个参数:clone到DIR处
    DIR=$2

    if [ ! -d ${DIR} ]; then
        echo "git clone ${GIT_REPO_URL} ${DIR}"
        git clone ${GIT_REPO_URL} ${DIR}
    else
        # 当前工作目录压入栈中,切换到DIR目录
        pushd ${DIR}
        echo "git pull origin master"
        git pull origin master
        # 切换到栈顶目录
        popd
    fi
}

function clone_or_update_from_git_with_submodule()
{
    GIT_REPO_URL=$1
    DIR=$2

    if [ ! -d ${DIR} ]; then
        echo "git clone ${GITREPO_URL} ${DIR}"
        git clone ${GIT_REPO_URL} ${DIR}
        git submodule init
        git submodule update
    else
        pushd ${DIR}
        echo "git pull origin master"
        git pull origin master
        git submodule init
        git submodule update
        popd
    fi
}

# args: project-name opts
function build_project()
{
    PROJECT=$1
    shift 1
    PROJECT_OPT=$*
    NAME=${PROJECT}-build
    mkdir -p ${NAME} && cd ${NAME}
    # 如crc32c库 会安装到 bitcask-cpp/third_party/中
    echo "cmake ${PROJECT_OPT} -DCMAKE_INSTALL_PREFIX=../../ ../${PROJECT}"
    cmake ${PROJECT_OPT} -DCMAKE_INSTALL_PREFIX=../../ ../${PROJECT}
    echo "make VERBOSE=1 && make install"
    make VERBOSE=1 && make install
    cd ../
}

function clear_build_history()
{
    # -p:如果build目录已存在则不会出错
    mkdir -p build && cd build/
    rm -rf *-build
    cd ../
}

# args: git_url project_name ops
function make_dep()
{
    PROJECT_URL=$1
    PROJECT_NAME=$2
    shift 2
    PROJECT_OPT=$*
    mkdir -p build && cd build/

    echo "clone/update ${PROJECT_NAME} from ${PROJECT_URL}..."
    clone_or_update_from_git ${PROJECT_URL} ${PROJECT_NAME} 

    echo "build ${PROJECT_NAME}..."
    build_project ${PROJECT_NAME} ${PROJECT_OPT}

    cd ../
}

function make_dep_with_submodule()
{
    PROJECT_URL=$1
    PROJECT_NAME=$2
    shift 2
    PROJECT_OPT=$*
    
    mkdir -p build && cd build/

    echo "clone/update with submodule ${PROJECT_NAME} from ${PROJECT_URL}..."
    clone_or_update_from_git_with_submodule ${PROJECT_URL} ${PROJECT_NAME}

    echo "build {PROJECT_NAME}..."
    build_project ${PROJECT_NAME} ${PROJECT_OPT}

    cd ../
}