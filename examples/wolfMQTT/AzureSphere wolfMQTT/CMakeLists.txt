#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.10)

project (AzureSphere_wolfMQTT C)

azsphere_configure_tools(TOOLS_REVISION "20.07")
azsphere_configure_api(TARGET_API_SET "6")

# Create executable
set(wolf wolfmqtt/src/)
add_library(MQTT ${wolf}mqtt_client.c ${wolf}mqtt_packet.c ${wolf}mqtt_socket.c)


add_executable (${PROJECT_NAME} main.c )
target_link_libraries (${PROJECT_NAME} applibs pthread gcc_s c MQTT)


azsphere_target_add_image_package(${PROJECT_NAME})
