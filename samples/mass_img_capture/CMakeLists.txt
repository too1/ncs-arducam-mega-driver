# SPDX-License-Identifier: Apache-2.0

# Manually add the arducam_mega module to the sample
list(APPEND ZEPHYR_EXTRA_MODULES
  ${CMAKE_CURRENT_SOURCE_DIR}/../../modules/arducam_mega
  )
  
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(mass)

if((NOT CONFIG_DISK_DRIVER_FLASH) AND
   (NOT CONFIG_DISK_DRIVER_RAM) AND
   (NOT CONFIG_DISK_DRIVER_SDMMC))
	message( FATAL_ERROR "No disk access settings detected." )
endif()

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
