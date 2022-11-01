# This file serves several purposes.
# It adds the directory it is contained in to CMakes module path, so that including it once
# lets CMake find FindPackage modules alongside it.
# Moreover, it defines the functions util_setup_target (and, if the shared library utils are present, util_setup_shared_library)
# that can be called on a target to setup certain flags and libraries for this target:
# - It enables warnings.
# - It sets the default visibility of symbols to hidden. Almost any code, even code compiled into a static library, lands in a
#      shared library or executable at some point; at the point of linking this final artifact, non-hidden symbols in a static library
#      might (depending on the platform) be exported.
# - In a release, relwithdebinfo or minsizerel build, when the option UTIL_ENABLE_HOST_SPECIFIC_OPTIMIZATIONS is set, it enables host-specific optimizations.
# - For all Boost modules in UTIL_BOOST_LIBRARIES, it calls FindPackage. util_setup_target then sets up the given target to use the given boost libraries.
# - It finds libraries needed for multi-threaded applications. util_setup_target links the given target against these libraries.
# - On platforms where this is necessary, it finds the libm and libdl libraries and links targets passed to util_setup_target against these libraries.
cmake_minimum_required(VERSION 3.4)

if(NOT COMMAND util_setup_target)

# make cmake export the compile commands to the build directory
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# try to figure out SVN revision or GIT commit ID/SHA hash for the current source dir (on first inclusion)
find_package(Subversion)
find_package(Git)

set(UTIL_HAVE_SVN_REVISION 0)
if(Subversion_FOUND)
	execute_process(COMMAND "${Subversion_SVN_EXECUTABLE}" info "${CMAKE_CURRENT_SOURCE_DIR}" RESULT_VARIABLE _UTIL_SVN_SUCCESSFUL OUTPUT_QUIET ERROR_QUIET)

	if("${_UTIL_SVN_SUCCESSFUL}" STREQUAL "0")
		# we are in an SVN repository
		add_custom_target(util_svn_header ALL)
		add_custom_command(
				TARGET util_svn_header
				COMMAND ${CMAKE_COMMAND}
					"-DGET_SVN_OUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/generated"
					"-DGET_SVN_REPOSITORY_PATH=${CMAKE_CURRENT_SOURCE_DIR}"
					-P "${CMAKE_CURRENT_LIST_DIR}/get_svn_revision.cmake"
		)

		set(UTIL_HAVE_SVN_REVISION 1)
		set(_UTIL_SVN_REVISION_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
	endif()
endif()

set(UTIL_HAVE_GIT_REVISION 0)
if(Git_FOUND)
	execute_process(COMMAND "${GIT_EXECUTABLE}" "rev-parse" "HEAD" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" RESULT_VARIABLE _UTIL_GIT_SUCCESSFUL OUTPUT_QUIET ERROR_QUIET)

	if("${_UTIL_GIT_SUCCESSFUL}" STREQUAL "0")
		# we are in a GIT repository
		add_custom_target(util_git_header ALL)
		add_custom_command(
				TARGET util_git_header
				COMMAND ${CMAKE_COMMAND}
					"-DGET_GIT_OUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}/generated"
					"-DGET_GIT_REPOSITORY_PATH=${CMAKE_CURRENT_SOURCE_DIR}"
					-P "${CMAKE_CURRENT_LIST_DIR}/get_git_revision.cmake"
		)

		set(UTIL_HAVE_GIT_REVISION 1)
		set(_UTIL_GIT_REVISION_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
	endif()
endif()

# enable target INTERFACE_LINK_LIBRARIES
cmake_policy(SET CMP0022 NEW)

if(CMAKE_MAJOR_VERSION GREATER 2 AND CMAKE_MINOR_VERSION GREATER 11)
	cmake_policy(SET CMP0074 NEW)
endif()

set(_UTIL_IPO_SUPPORTED FALSE)
if(${CMAKE_MAJOR_VERSION} GREATER 2 AND ${CMAKE_MINOR_VERSION} GREATER 8)
	cmake_policy(SET CMP0069 NEW)
	include(CheckIPOSupported)
	check_ipo_supported(RESULT _UTIL_IPO_SUPPORTED OUTPUT _UTIL_IPO_UNSUPPORTED_ERROR LANGUAGES CXX)

	if(NOT _UTIL_IPO_SUPPORTED)
		message(STATUS "Interprocedural/link-time optimization not supported: ${_UTIL_IPO_UNSUPPORTED_ERROR}")
	endif()
endif()

set(UTIL_EXTRA_COMPILE_FLAGS_DEBUG "" CACHE STRING "Extra flags to pass to the C/C++-compiler in non-optimized builds (CMake list format, i.e., ;-separated).")
set(UTIL_EXTRA_COMPILE_FLAGS_RELEASE "" CACHE STRING "Extra flags to pass to the C/C++-compiler in non-optimized builds (CMake list format, i.e., ;-separated).")
option(UTIL_USE_ASAN "Whether to turn compilation with the address sanitizer on or off (on GCC or clang, in Debug mode only)." Off)

set(_UTIL_FLAGS "")
set(_UTIL_FLAGS_D "")
set(_UTIL_FLAGS_R "")
set(_UTIL_LD_FLAGS "")
set(_UTIL_LD_FLAGS_D "")

# store the current list dir for later use
set(_UTIL_HELPER_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

# add this directory to the module path so our modules can be found
set(CMAKE_MODULE_PATH ${_UTIL_HELPER_DIRECTORY} ${CMAKE_MODULE_PATH})

# enable warnings
if(MSVC)
	set(_UTIL_FLAGS "${_UTIL_FLAGS};/W4;/wd4127;/wd4244;/wd4701")
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
	set(_UTIL_FLAGS "${_UTIL_FLAGS};-Wall;-Wextra;-Wno-long-long;-Wno-missing-braces")
else()
	message(STATUS "Could not add warnings; unrecognized compiler id '${CMAKE_CXX_COMPILER_ID}'!")
endif()

# handle ASAN flag
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
	if(UTIL_USE_ASAN)
		set(_UTIL_FLAGS_D "${_UTIL_FLAGS_D};-fsanitize=address")
		set(_UTIL_LD_FLAGS_D "${_UTIL_LD_FLAGS_D} -fsanitize=address")
	endif()
endif()

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	option(UTIL_ENABLE_HOST_SPECIFIC_OPTIMIZATIONS "Optimize for the platform/system the build process runs on." On)

	if(UTIL_ENABLE_HOST_SPECIFIC_OPTIMIZATIONS)
		set(_UTIL_FLAGS_R "${_UTIL_FLAGS_R};-march=native;-mtune=native")
	endif()
endif()

# on Apple, use the modern Clang assembler (otherwise the compiler might spit out assembler code the assembler does not understand)
if(UNIX AND APPLE AND CMAKE_COMPILER_IS_GNUCXX)
	set(_UTIL_FLAGS "${_UTIL_FLAGS};-Wa,-q")
endif()

# Some further linker flags
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	if(NOT WIN32 AND NOT APPLE)
		set(_UTIL_LD_FLAGS "${_UTIL_LD_FLAGS} -Wl,--as-needed -Wl,--hash-style=gnu -Wl,--exclude-libs=ALL")
	endif()
endif()

if(NOT "${UTIL_EXTRA_COMPILE_FLAGS_DEBUG}" STREQUAL "")
	set(_UTIL_FLAGS_D "${_UTIL_FLAGS_D};${UTIL_EXTRA_COMPILE_FLAGS_DEBUG}")
endif()

if(NOT "${UTIL_EXTRA_COMPILE_FLAGS_RELEASE}" STREQUAL "")
	set(_UTIL_FLAGS_R "${_UTIL_FLAGS_R};${UTIL_EXTRA_COMPILE_FLAGS_RELEASE}")
endif()

# allow the user a simple way to link everything against additional libraries
set(UTIL_ADDITIONAL_LIBRARIES "" CACHE STRING "A ';'-separated list of additional libraries to link all targets passed to util_setup_target to.")
set(UTIL_ADDITIONAL_INCLUDE_DIRS "" CACHE STRING "A ';'-separated list of additional libraries to add to the include directories of all targets passed to util_setup_target.")
mark_as_advanced(UTIL_ADDITIONAL_LIBRARIES UTIL_ADDITIONAL_INCLUDE_DIRS)
set(_UTIL_LIBRARIES ${_UTIL_LIBRARIES} ${UTIL_ADDITIONAL_LIBRARIES})
set(_UTIL_INCLUDE_DIRS ${UTIL_ADDITIONAL_INCLUDE_DIRS})

# threading support; for instance, this pulls in pthread on Linux
find_package(Threads REQUIRED)
set(_UTIL_LIBRARIES ${_UTIL_LIBRARIES} Threads::Threads)

# search for required boost libraries
if(UTIL_BOOST_LIBRARIES)
    mark_as_advanced(UTIL_BOOST_LIBRARIES)

	# if there are multiple boost variants, definitely go for the multithreaded one
	set(Boost_USE_MULTITHREADED ON)

	find_package(Boost REQUIRED COMPONENTS ${UTIL_BOOST_LIBRARIES})
	set(_UTIL_LIBRARIES ${_UTIL_LIBRARIES} ${Boost_LIBRARIES})
	set(_UTIL_INCLUDE_DIRS ${_UTIL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
endif()

# Generally, on Linux, the math library may be required
if(UNIX)
	option(UTIL_MATH_LIBRARY "Link against the standard math library libm if it is present." On)
	find_library(_UTIL_MATH_LIBRARY NAMES m)
	if(_UTIL_MATH_LIBRARY)
		set(_UTIL_LIBRARIES ${_UTIL_LIBRARIES} ${_UTIL_MATH_LIBRARY})
	endif()
	mark_as_advanced(UTIL_MATH_LIBRARY _UTIL_MATH_LIBRARY)
endif()

# Sometimes, on Linux, the rt library may be required
if(UNIX AND NOT APPLE)
	option(UTIL_RT_LIBRARY "Link against the RT library if it is present." On)
	mark_as_advanced(UTIL_RT_LIBRARY)
	find_library(_UTIL_RT_LIBRARY NAMES rt)
	if(_UTIL_RT_LIBRARY)
		set(_UTIL_LIBRARIES ${_UTIL_LIBRARIES} ${_UTIL_RT_LIBRARY})
	endif()
endif()

# In many cases, a dynamic linking library is necessary (libdl on Linux)
if(UNIX)
	find_library(_UTIL_LIBDL_LIBRARY NAMES dl)
	if(_UTIL_LIBDL_LIBRARY)
		set(_UTIL_LIBRARIES ${_UTIL_LIBRARIES} ${_UTIL_LIBDL_LIBRARY})
	endif()
    mark_as_advanced(_UTIL_LIBDL_LIBRARY)
endif()

# use the util_setup_target function to add libraries and include dirs found/setup here to the given target;
# it takes additional optional parameters as follows:
# LIBRARIES <list of-libraries> INCLUDE_DIRS <list-of-include-dirs>
function(util_setup_target TARGETNAME)
	cmake_parse_arguments(_UST "" "" "LIBRARIES;INCLUDE_DIRS" ${ARGN})

	if(NOT "${_UTIL_LIBRARIES}" STREQUAL "")
		target_link_libraries(${TARGETNAME} PRIVATE ${_UTIL_LIBRARIES})
	endif()

	if(NOT "${_UST_LIBRARIES}" STREQUAL "")
		target_link_libraries(${TARGETNAME} PUBLIC ${_UST_LIBRARIES})
	endif()
	
	if(NOT "${_UTIL_INCLUDE_DIRS}" STREQUAL "")
		target_include_directories(${TARGETNAME} PUBLIC ${_UTIL_INCLUDE_DIRS})
	endif()

	if(NOT "${_UST_INCLUDE_DIRS}" STREQUAL "")
		target_include_directories(${TARGETNAME} PUBLIC ${_UST_INCLUDE_DIRS})
	endif()

	if(UTIL_HAVE_SVN_REVISION)
		target_include_directories(${TARGETNAME} PUBLIC ${_UTIL_SVN_REVISION_INCLUDE_DIR})
		target_compile_definitions(${TARGETNAME} PUBLIC "ALGUTIL_HAVE_SVN_REVISION=1")
		add_dependencies(${TARGETNAME} util_svn_header)
	endif()

	if(UTIL_HAVE_GIT_REVISION)
		target_include_directories(${TARGETNAME} PUBLIC ${_UTIL_GIT_REVISION_INCLUDE_DIR})
		target_compile_definitions(${TARGETNAME} PUBLIC "ALGUTIL_HAVE_GIT_REVISION=1")
		add_dependencies(${TARGETNAME} util_git_header)
	endif()

	if(NOT WIN32)
	        set_target_properties(${TARGETNAME} PROPERTIES CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN On C_VISIBILITY_PRESET hidden)
	endif()

	if(_UTIL_IPO_SUPPORTED AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
		set_target_properties(${TARGETNAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
	endif()

	if(NOT "${_UTIL_FLAGS}" STREQUAL "")
		target_compile_options(${TARGETNAME} PRIVATE ${_UTIL_FLAGS})
	endif()

	if(NOT "${_UTIL_FLAGS_D}" STREQUAL "")
		target_compile_options(${TARGETNAME} PRIVATE "$<$<CONFIG:Debug>:${_UTIL_FLAGS_D}>")
	endif()

	if(NOT "${_UTIL_FLAGS_R}" STREQUAL "")
		target_compile_options(${TARGETNAME} PRIVATE "$<$<NOT:$<CONFIG:Debug>>:${_UTIL_FLAGS_R}>")
	endif()

	get_target_property(_EXISTING_LINK_FLAGS ${TARGETNAME} LINK_FLAGS)
	if(NOT _EXISTING_LINK_FLAGS)
		set(_EXISTING_LINK_FLAGS "")
	endif()

	get_target_property(_EXISTING_LINK_FLAGS_DEBUG ${TARGETNAME} LINK_FLAGS_DEBUG)
	if(NOT _EXISTING_LINK_FLAGS_DEBUG)
		set(_EXISTING_LINK_FLAGS_DEBUG "")
	endif()

	set_target_properties(
		${TARGETNAME} PROPERTIES
			LINK_FLAGS "${_EXISTING_LINK_FLAGS} ${_UTIL_LD_FLAGS}"
			LINK_FLAGS_DEBUG "${_EXISTING_LINK_FLAGS_DEBUG} ${_UTIL_LD_FLAGS_D}"
	)
endfunction(util_setup_target)

# generate the header file "shared_library_util.h" (compatible to both C and C++) in the directory OUTPUTDIR;
# this header should be included in every compilation unit (ideally in config header) of the shared library.
# the macros "toupper(${LIBNAME})_API_(PUBLIC|PRIVATE)" can then be used to hide/export symbols.
function(util_setup_shared_library TARGETNAME LIBNAME OUTPUTDIR)
	# generate header file (needs to know UPPERCASE_TARGETNAME to make symbol unique)
	string(TOUPPER "${LIBNAME}" UPPERCASE_TARGETNAME)
	configure_file("${_UTIL_HELPER_DIRECTORY}/shared_library_util.h" "${OUTPUTDIR}/${LIBNAME}/shared_library_util.h")
	
	# compile-time definition when building the shared library (DO NOT DEFINE THIS WHEN LINKING AGAINST SHARED LIBRARY)
	set(DEFINE_SHARED_SYMBOL "${UPPERCASE_TARGETNAME}_BUILD_SHARED")
	target_compile_definitions(${TARGETNAME} PRIVATE "${DEFINE_SHARED_SYMBOL}=1")

	# bring the hexagonal header into the include path
	target_include_directories(${TARGETNAME} PRIVATE "${OUTPUTDIR}")
	set_target_properties(${TARGETNAME} PROPERTIES OUTPUT_NAME ${LIBNAME} DIRECT_OUTPUT 1)
endfunction(util_setup_shared_library)

# if util_setup_shared_library is used and a static version of the library is built, use util_setup_static_library on the static library target
function(util_setup_static_library TARGETNAME LIBNAME)
	string(TOUPPER "${LIBNAME}" UPPERCASE_TARGETNAME)
	set(DEFINE_STATIC_SYMBOL "${UPPERCASE_TARGETNAME}_STATIC")
	target_compile_definitions(${TARGETNAME} PUBLIC "${DEFINE_STATIC_SYMBOL}=1")
	set_target_properties(${TARGETNAME} PROPERTIES OUTPUT_NAME ${LIBNAME} DIRECT_OUTPUT 1)
endfunction(util_setup_static_library)

# used to create an imported target for external libraries
function(util_add_imported_library TARGETNAME LOCATION INCLUDE_DIRS)
	add_library(${TARGETNAME} UNKNOWN IMPORTED)

	string(REGEX MATCH "debug;([^;]+);optimized;([^;]+)" _DEBOPT_MATCHED "${LOCATION}")
	if(_DEBOPT_MATCHED)
		set_target_properties(${TARGETNAME} PROPERTIES
			IMPORTED_LOCATION "${CMAKE_MATCH_2}"
			IMPORTED_LOCATION_RELEASE "${CMAKE_MATCH_2}"
			IMPORTED_LOCATION_MINSIZEREL "${CMAKE_MATCH_2}"
			IMPORTED_LOCATION_RELWITHDEBINFO "${CMAKE_MATCH_2}"
			IMPORTED_LOCATION_DEBUG "${CMAKE_MATCH_1}"
		)
	else()
		string(REGEX MATCH "optimized;([^;]+);debug;([^;]+)" _DEBOPT_MATCHED "${LOCATION}")
		if(_DEBOPT_MATCHED)
			set_target_properties(${TARGETNAME} PROPERTIES
				IMPORTED_LOCATION "${CMAKE_MATCH_1}"
				IMPORTED_LOCATION_RELEASE "${CMAKE_MATCH_1}"
				IMPORTED_LOCATION_MINSIZEREL "${CMAKE_MATCH_1}"
				IMPORTED_LOCATION_RELWITHDEBINFO "${CMAKE_MATCH_1}"
				IMPORTED_LOCATION_DEBUG "${CMAKE_MATCH_2}"
			)
		else()
			set_target_properties(${TARGETNAME} PROPERTIES IMPORTED_LOCATION "${LOCATION}")
		endif()
	endif()
	
	if(NOT "${INCLUDE_DIRS}" STREQUAL "")
		set_target_properties(${TARGETNAME} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${INCLUDE_DIRS}")
	endif()
endfunction()

# used to add libraries (preferrably imported targets) to an imported library
function(util_imported_link_libraries TARGET LIBRARIES)
	get_target_property(_EXISTING_LIBS ${TARGET} INTERFACE_LINK_LIBRARIES)
	if(NOT _EXISTING_LIBS)
		set(_EXISTING_LIBS "")
	endif()

	foreach(LIBNAME IN LISTS LIBRARIES)
		if(NOT "${LIBNAME}" STREQUAL "")
			set(_EXISTING_LIBS "${_EXISTING_LIBS};${LIBNAME}")

			if(TARGET ${LIBNAME})
				get_target_property(_LIBNAME_INTERFACE_LIBS ${LIBNAME} INTERFACE_LINK_LIBRARIES)
				if(_LIBNAME_INTERFACE_LIBS)
					set(_EXISTING_LIBS "${_EXISTING_LIBS};${_LIBNAME_INTERFACE_LIBS}")
				endif()
			endif()
		endif()
	endforeach(LIBNAME)

	set_target_properties(${TARGET} PROPERTIES INTERFACE_LINK_LIBRARIES "${_EXISTING_LIBS}")
endfunction()

function(util_enable_gtest)
	if(NOT TARGET util-gtest-base)
		enable_testing()
		include(CTest)

		set(_GTEST_DIR "${_UTIL_HELPER_DIRECTORY}/gtest/googletest")

		if(NOT IS_DIRECTORY ${_GTEST_DIR})
			set(GTEST_FOUND FALSE CACHE STRING "Whether the Google Test library was found.")
		else()
			set(GTEST_FOUND TRUE CACHE STRING "Whether the Google Test library was found.")

            add_library(util-gtest-base INTERFACE IMPORTED)
            set_target_properties(util-gtest-base PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${_GTEST_DIR}/include")

            add_library(util-gtest-all STATIC "${_GTEST_DIR}/src/gtest-all.cc")
			util_setup_target(util-gtest-all)
			target_include_directories(util-gtest-all PUBLIC "${_GTEST_DIR}/include")
			target_include_directories(util-gtest-all PRIVATE "${_GTEST_DIR}")
			target_link_libraries(util-gtest-all PRIVATE util-gtest-base)

			add_library(util-gtest-main STATIC "${_GTEST_DIR}/src/gtest_main.cc")
			util_setup_target(util-gtest-main)
			target_include_directories(util-gtest-main PUBLIC "${_GTEST_DIR}/include")
			target_include_directories(util-gtest-main PRIVATE "${_GTEST_DIR}")
			target_link_libraries(util-gtest-main PRIVATE util-gtest-base)
		endif()
	endif()
endfunction()

function(util_create_testcase TARGETNAME SOURCES)
	util_enable_gtest()
    if(GTEST_FOUND)
		add_executable(${TARGETNAME} ${SOURCES})
		util_setup_target(${TARGETNAME} ${ARGN})
		target_link_libraries(${TARGETNAME} PRIVATE util-gtest-all util-gtest-main)
        add_test(NAME ${TARGETNAME} COMMAND ${TARGETNAME})
	else()
		message(WARNING "Google Test could not be found - testcase '${TARGETNAME}' will not be created!")
	endif()
endfunction()

endif()
