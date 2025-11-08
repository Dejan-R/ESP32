# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly


# Glavni ulaz u build sustav ESP-IDF
cmake_minimum_required(VERSION 3.16)
# Uključuje ESP-IDF build sistem (postavlja toolchain, putanje, komponente)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(00_ESP32_Project_Structure)

