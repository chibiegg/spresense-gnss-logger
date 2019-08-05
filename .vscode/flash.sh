#!/bin/bash

# Temporary
# Get platform type
case "$(uname -s)" in
	Linux*)
		PLATFORM=linux
		;;
	Darwin*)
		PLATFORM=macos
		;;
	CYGWIN*|MINGW32*|MSYS*)
		PLATFORM=windows
		;;
	*)
		echo "ERROR: Unknown platform"
		echo ""
		show_help
		;;
esac

if [ "${1}" == "flash" ]; then
    if [ "${ISAPPFOLDER}" == "true" ]; then
        export SPRESENSE_HOME=`pwd`
        PROJECT_NAME=`basename ${SPRESENSE_HOME}`
        cd ${SDK_PATH}/sdk
        ./tools/flash.sh -c ${PORT} ${SPRESENSE_HOME}/out/${PROJECT_NAME}.nuttx.spk
    else
        cd ${SDK_PATH}/sdk
        ./tools/flash.sh -c ${PORT} nuttx.spk
    fi
elif [ "${1}" == "clean" ]; then
    cd ${SDK_PATH}/sdk
    ./tools/${PLATFORM}/flash_writer -s -c ${PORT} -d -e nuttx
fi
