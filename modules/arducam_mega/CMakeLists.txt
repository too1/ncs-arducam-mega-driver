# SPDX-License-Identifier: Apache-2.0

add_subdirectory_ifdef(CONFIG_ARDUCAM_MEGA lib)
target_include_directories(app PRIVATE include/arducam_mega)