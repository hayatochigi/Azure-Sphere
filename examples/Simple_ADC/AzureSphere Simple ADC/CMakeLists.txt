#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.10)

project (AzureSphere_Simple_ADC C)

azsphere_configure_tools(TOOLS_REVISION "20.07")
azsphere_configure_api(TARGET_API_SET "6")

# Create executable
add_executable (${PROJECT_NAME} main.c)
target_link_libraries (${PROJECT_NAME} applibs pthread gcc_s c)

# hardware_definition.hが配置されているフォルダ = TARGET_DIRECTORY
# TARGET_DEFINITIONは、作成したjsonファイル
azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "Hardware" TARGET_DEFINITION "hardware_definition.json")

azsphere_target_add_image_package(${PROJECT_NAME})
