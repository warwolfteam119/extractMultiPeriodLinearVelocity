cmake_minimum_required(VERSION 3.23)
project(extractMultiPeriodLinearVelocity C)

set(CMAKE_C_STANDARD 99)


set(FFTW_INCLUDE_DIR "/opt/homebrew/opt/fftw/include")
set(FFTW_LIBRARY "/opt/homebrew/opt/fftw/lib/libfftw3.dylib")
set(portaudio_INCLUDE_DIR "/opt/homebrew/opt/portaudio/include")
set(portaudio_LIBRARY "/opt/homebrew/opt/portaudio/lib/libportaudio.dylib")
set(OPENBLAS_INCLUDE_DIR "/opt/homebrew/opt/openblas/include")
set(OPENBLAS_LIBRARY "/opt/homebrew/opt/openblas/lib/libblas.dylib")
add_executable(extractMultiPeriodLinearVelocity main.c featurize.h featurize.c test.c test.h)

include_directories(${FFTW_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${FFTW_LIBRARY})
include_directories(${OPENBLAS_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${OPENBLAS_LIBRARY})
include_directories(${portaudio_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${portaudio_LIBRARY})

