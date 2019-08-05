#!/bin/bash

# Name: do_kernelbuild
# Description: Build Kernel
# Usage: do_kernelbuild <Project folder's .config> <SDK folder's .config>
# If <Project folder's .config> = <SDK folder's .config>, will not update .config file
function do_kernelbuild (){
    # App folder config
    APP_KERNEL_CFG=$1

    # SDK folder config
    KERNEL_CFG=$2

    # Check target config file
    if [ ! -f "${APP_KERNEL_CFG}" ]; then
        echo "Kernel is not configured. Please use "Spresense: Kernel config""
        exit 1
    fi

    # Check Kernel config
    if [ ! -f "${KERNEL_CFG}" -o "`diff ${APP_KERNEL_CFG} ${KERNEL_CFG}`" != "" ]; then
        # Update to new configuration file
        cp ${APP_KERNEL_CFG} ${KERNEL_CFG}
    fi

    # Check Make.defs and copy it if not exist
    if [ ! -f "${SDK_PATH}/nuttx/Make.defs" ]; then
        cp -a ${SDK_PATH}/sdk/bsp/scripts/Make.defs.nuttx ${SDK_PATH}/nuttx/Make.defs
    fi

    cd ${SDK_PATH}/sdk
    make buildkernel
}

# Name: do_build
# Description: Build SDK with application
# Usage: do_build <Project folder's .config> <SDK folder's .config>
# If <Project folder's .config> = <SDK folder's .config>, will not update .config file
function do_build (){
    # App folder config
    APPS_SDK_CFG=$1

    # SDK folder config
    SDK_CFG=$2

    # Check target config file
    if [ ! -f "${APPS_SDK_CFG}" ]; then
        echo "SDK is not configured. Please use "Spresense: SDK config""
        exit 1
    fi

    # Check SDK config
    if [ ! -f "${SDK_CFG}" -o "`diff ${APPS_SDK_CFG} ${SDK_CFG}`" != "" ]; then
        # Update to new configuration file
        cp ${APPS_SDK_CFG} ${SDK_CFG}
    fi

    cd ${SDK_PATH}/sdk
    make 
}

if [ "${ISAPPFOLDER}" == "true" ]; then
    export SPRESENSE_HOME=`pwd`
    PROJECT_NAME=`basename ${SPRESENSE_HOME}`

    # Build kernel first
    do_kernelbuild ${SPRESENSE_HOME}/kernel.config ${SDK_PATH}/nuttx/.config

    # Build SDK with app
    do_build ${SPRESENSE_HOME}/sdk.config ${SDK_PATH}/sdk/.config

    # Copy output files
    mkdir -p ${SPRESENSE_HOME}/out
	cp -a ${SDK_PATH}/sdk/nuttx.spk ${SPRESENSE_HOME}/out/${PROJECT_NAME}.nuttx.spk
	cp -a ${SDK_PATH}/sdk/nuttx ${SPRESENSE_HOME}/out/${PROJECT_NAME}.nuttx
else
    # Build kernel first
    do_kernelbuild ${SDK_PATH}/nuttx/.config ${SDK_PATH}/nuttx/.config

    # Build SDK with app
    do_build ${SDK_PATH}/sdk/.config ${SDK_PATH}/sdk/.config
fi
