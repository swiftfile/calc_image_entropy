cmake_minimum_required(VERSION 3.23)
project(entropy)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)

include_directories(
        ${OpenCV_INCLUDE_DIRS}
)

add_executable(entropy
        entropy/main.cpp
        )
add_executable(fft-sobel
        fft-sobel/main.cpp
        )

add_executable(dynamicfft
        sample/dynamicfft/main.cpp
        )
add_executable(distance
        distance/main.cpp
        )

target_link_libraries(
        entropy
        ${OpenCV_LIBRARIES}
)
target_link_libraries(
        fft-sobel
        ${OpenCV_LIBRARIES}
)

target_link_libraries(
        dynamicfft
        ${OpenCV_LIBRARIES}
)
target_link_libraries(
        distance
        ${OpenCV_LIBRARIES}
)