# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

set( CPATH src/main/cpp/ )
set(distribution_DIR ${CMAKE_SOURCE_DIR}/distribution/)
set(PACKAGE ${CPATH}package/ )
set(GMP ${PACKAGE}gmp/ )
set(NONGMP ${PACKAGE}double/ )

add_library( libgmp
             SHARED
             IMPORTED )
set_target_properties( # Specifies the target library.
                       libgmp

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${distribution_DIR}gmp/${ANDROID_ABI}/libgmp.so )



add_library( # Sets the name of the library.
             imagecipher1

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${NONGMP}imagecipher1.h
             ${NONGMP}imagecipher1.c
             )

add_library( # Sets the name of the library.
             imagecipher1gmp

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${GMP}imagecipher1.h
             ${GMP}imagecipher1.c
             )

add_library( # Sets the name of the library.
             imagecipher2

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${NONGMP}imagecipher2.h
             ${NONGMP}imagecipher2.c
             )

add_library( # Sets the name of the library.
             imagecipher2gmp

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${GMP}imagecipher2.h
             ${GMP}imagecipher2.c
             )

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${CPATH}native-lib.cpp )

add_library( # Sets the name of the library.
             native-lib-gmp

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${CPATH}native-lib-gmp.cpp )

add_library( # Sets the name of the library.
             native-lib-aes

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${CPATH}native-lib-aes.cpp )

add_library( # Sets the name of the library.
             aes

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${CPATH}tinyaes2/aes.h
             ${CPATH}tinyaes2/aes.c
             )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       aes
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )

target_link_libraries( # Specifies the target library.
                       imagecipher1
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )

 target_link_libraries( # Specifies the target library.
                        imagecipher2
                        # Links the target library to the log library
                        # included in the NDK.
                        ${log-lib} )

 target_link_libraries( # Specifies the target library.
                        imagecipher1gmp
                        # Links the target library to the log library
                        # included in the NDK.
                         libgmp
                        ${log-lib} )

  target_link_libraries( # Specifies the target library.
                         imagecipher2gmp
                         # Links the target library to the log library
                         # included in the NDK.
                         libgmp
                         ${log-lib} )

target_link_libraries( # Specifies the target library.
                       native-lib
                       imagecipher1
                       imagecipher2
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )

target_include_directories(native-lib-gmp PRIVATE
                           ${distribution_DIR}gmp/include)

target_link_libraries( # Specifies the target library.
                       native-lib-gmp
                       imagecipher1gmp
                       imagecipher2gmp
                       libgmp
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )

target_link_libraries( # Specifies the target library.
                      native-lib-aes
                      aes
                      # Links the target library to the log library
                      # included in the NDK.
                      ${log-lib} )