#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)
COMPONENT_ADD_INCLUDEDIRS += ../../_components
GIT_PROJECT_VER := $(shell cd ${PROJECT_PATH} && git describe --always --tags --dirty 2> /dev/null)