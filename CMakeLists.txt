# SPDX-License-Identifier: Apache-2.0

# Manually add the app_uart module to the sample
list(APPEND ZEPHYR_EXTRA_MODULES
  ${CMAKE_CURRENT_SOURCE_DIR}/modules/arducam_mega
  )

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app_uart_test)

target_include_directories(app PRIVATE src)
target_sources(app PRIVATE src/main.c)