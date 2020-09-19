#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.10)

project (AzureSphere_IoT_Hub C)

azsphere_configure_tools(TOOLS_REVISION "20.07")
azsphere_configure_api(TARGET_API_SET "6")

# Create executable
add_executable (${PROJECT_NAME} main.c)
# target_link_librariesに"asureiot"を追加
target_link_libraries (${PROJECT_NAME} applibs pthread gcc_s c azureiot)

# IoT Hubサンプルのために新規に追加した2行
target_include_directories(${PROJECT_NAME} PUBLIC ${AZURE_SPHERE_API_SET_DIR}/usr/include/azureiot ${AZURE_SPHERE_API_SET_DIR}/usr/include/azure_prov_client)
target_compile_definitions(${PROJECT_NAME} PUBLIC AZURE_IOT_HUB_CONFIGURED)

azsphere_target_add_image_package(${PROJECT_NAME})
