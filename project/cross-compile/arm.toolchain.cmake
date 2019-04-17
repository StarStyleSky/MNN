# ====================================================
#   Copyright (C)2018 All rights reserved.
#
#   Author        : axel.hb
#   Email         : axel.hb@alibaba-inc.com
#
# ====================================================*/

# set target ABI options
if( ARM_NAME STREQUAL "hisi3519v101" )
	set( CMAKE_SYSTEM_NAME Linux )
	set( CMAKE_SYSTEM_PROCESSOR armv7-a )
	set( CMAKE_C_COMPILER arm-hisiv500-linux-gcc)
	set( CMAKE_CXX_COMPILER arm-hisiv500-linux-g++)
	add_definitions(-mcpu=cortex-a17.cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -mno-unaligned-access -fno-aggressive-loop-optimizations -ffunction-sections -fdata-sections)
elseif( ARM_NAME STREQUAL "hisi3516d100" )
	set( CMAKE_SYSTEM_NAME Linux )
	set( CMAKE_SYSTEM_PROCESSOR armv7-a )
	set( CMAKE_C_COMPILER arm-hisiv300-linux-gcc)
	set( CMAKE_CXX_COMPILER arm-hisiv300-linux-g++)
	add_definitions(-mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4)
elseif( ARM_NAME STREQUAL "hisi3518e200" )
	set( CMAKE_SYSTEM_NAME Linux )
	set( CMAKE_SYSTEM_PROCESSOR armv5 )
	set( CMAKE_C_COMPILER arm-hisiv300-linux-gcc)
	set( CMAKE_CXX_COMPILER arm-hisiv300-linux-g++)
	add_definitions()
elseif( ARM_NAME STREQUAL "arm-gnueabihf" )
	set( CMAKE_SYSTEM_NAME Linux )
	set( CMAKE_SYSTEM_PROCESSOR armv7-a )
	set( CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
	set( CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
	add_definitions( -mfloat-abi=hard -fsigned-char -marm -mfpu=neon -march=armv7-a -fvisibility=hidden)
elseif( ARM_NAME STREQUAL "aarch64-gnueabihf" )
	set( CMAKE_SYSTEM_NAME Linux )
	set( CMAKE_SYSTEM_PROCESSOR aarch64v )
	set( CMAKE_C_COMPILER aarch64-linux-gnueabihf-gcc)
	set( CMAKE_CXX_COMPILER aarch64-linux-gnueabihf-g++)
	add_definitions()
endif()
