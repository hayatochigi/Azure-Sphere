#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.10)

project (AzureSphereHTTP C)

azsphere_configure_tools(TOOLS_REVISION "20.07")
azsphere_configure_api(TARGET_API_SET "6")

# Create executable
add_executable (${PROJECT_NAME} main.c)

# parsonライブラリを追加
add_library(json_parser json/parson.c json/parson.h)

# インクルードディレクトリにjsonフォルダを追加
include_directories(${PROJECT_NAME} json)

# "curl"を追加
target_link_libraries (${PROJECT_NAME} applibs pthread gcc_s c curl json_parser)

azsphere_target_add_image_package(${PROJECT_NAME})
