# SPDX-License-Identifier: Apache-2.0

if(CONFIG_ARDUCAM_MEGA)
	zephyr_include_directories(../../include/arducam_mega)
	zephyr_library()
	zephyr_library_sources(
	  arducam_mega.c
	  )
  endif()