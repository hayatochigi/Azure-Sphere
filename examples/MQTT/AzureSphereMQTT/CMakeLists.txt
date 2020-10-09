#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.10)

project (AzureSphereMQTT C)

azsphere_configure_tools(TOOLS_REVISION "20.07")
azsphere_configure_api(TARGET_API_SET "6")

# MQTTライブラリを追加
add_library(mqtt 
			src/mqtt.c
			src/mqtt_pal.c
			)


# インクルードディレクトリを追加
include_directories(&{PROJECT_NAME} include)

# Create executable
add_executable (${PROJECT_NAME} main.c)
target_link_libraries (${PROJECT_NAME} applibs pthread gcc_s c mqtt)

azsphere_target_add_image_package(${PROJECT_NAME})
