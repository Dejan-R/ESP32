# Glavni ulaz u build sustav ESP-IDF
cmake_minimum_required(VERSION 3.16)
project(ESP32_project_structure)

# Uključuje ESP-IDF build sistem (postavlja toolchain, putanje, komponente)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)