include(FindPackageHandleStandardArgs)

if(NOT CPLEX_ROOT)
	set(CPLEX_ROOT "" CACHE STRING "The root directory of your CPLEX Optimization Studio installation.")
endif()

# if CPLEX_ROOT is not given, try to find CPLEX using the binary which hopefully resides in the path
if(NOT CPLEX_ROOT)
	find_program(_CPLEX_EXECUTABLE_BINARY cplex)
    mark_as_advanced(_CPLEX_EXECUTABLE_BINARY)

	if(NOT _CPLEX_EXECUTABLE_BINARY)
		set(_CPLEX_ROOT_GUESSES "/opt/tmp/ibm/ILOG;/opt/IBM/ILOG;/opt/ibm/ILOG;/opt/ibm/ilog;/Applications;C:/Program Files/IBM/ILOG;C:/Program Files (x86)/IBM/ILOG")
		
		foreach(CPLEX_ROOT_GUESS IN LISTS _CPLEX_ROOT_GUESSES)
			if(IS_DIRECTORY ${CPLEX_ROOT_GUESS})
				if(NOT FIND_CPLEX_QUIETLY)
					message(STATUS "Searching '${CPLEX_ROOT_GUESS}' for cplex (this may take a while)...")
				endif()
				FILE(GLOB_RECURSE _CPLEX_FILES "${CPLEX_ROOT_GUESS}/*cplex")
				foreach(F IN LISTS _CPLEX_FILES)
					if(EXISTS ${F} AND NOT IS_DIRECTORY ${F})
						set(_CPLEX_EXECUTABLE_BINARY ${F})
						break()
					endif()
				endforeach()
				if(_CPLEX_EXECUTABLE_BINARY)
					break()
				endif()
			endif()
		endforeach()
	endif()

	if(_CPLEX_EXECUTABLE_BINARY)
		if(NOT CPLEX_FIND_QUIETLY)
			message(STATUS "Found cplex command-line binary: '${_CPLEX_EXECUTABLE_BINARY}'")
		endif()

		# get directory component
		get_filename_component(_CPLEX_EXECUTABLE_DIR ${_CPLEX_EXECUTABLE_BINARY} DIRECTORY)

		# get CPLEX_ROOT
		string(REGEX MATCH "^(.+)/cplex/(.*)$" CPLEX_MATCH_OUTPUT_ ${_CPLEX_EXECUTABLE_DIR})
		if(CPLEX_MATCH_OUTPUT_)
			if(NOT CPLEX_FIND_QUIETLY)
				message(STATUS "CPLEX_ROOT determined by cplex executable: ${CMAKE_MATCH_1}")
			endif()
			set(CPLEX_ROOT ${CMAKE_MATCH_1})
		endif()
	else()
		if(NOT CPLEX_ROOT AND NOT _CPLEX_EXECUTABLE_BINARY)
			message(STATUS "CPLEX binary was not found (is it on the path?) and CPLEX_ROOT was not given. Finding CPLEX will most likely fail.")
		endif()
	endif()
endif()

if(NOT CPLEX_ROOT)
	if(NOT CPLEX_FIND_QUIETLY)
		message(STATUS "CPLEX_ROOT is not set. Finding CPLEX will most likely fail.")
	endif()
endif()

find_path(
	_ILCPLEX_INCLUDE_DIR
	NAMES
		ilcplex/cplex.h
	HINTS
		"${CPLEX_ROOT}/include"
		"${CPLEX_ROOT}/cplex/include"
	PATH_SUFFIXES
		cplex
)
mark_as_advanced(_ILCPLEX_INCLUDE_DIR)

if(NOT _ILCPLEX_INCLUDE_DIR)
	if(NOT CPLEX_FIND_QUIETLY)
		message(STATUS "Could not find include directory for ilcplex/cplex.h")
	endif()
else()
	if(NOT CPLEX_FIND_QUIETLY)
		message(STATUS "CPLEX include directory for ilcplex/cplex.h: ${_ILCPLEX_INCLUDE_DIR}")
	endif()
endif()

find_path(
	_ILCONCERT_INCLUDE_DIR
	NAMES
		ilconcert/iloenv.h
	HINTS
		${_ILCPLEX_INCLUDE_DIR}
		"${CPLEX_ROOT}/include"
		"${CPLEX_ROOT}/concert/include"
	PATH_SUFFIXES
		cplex
		concert
)
mark_as_advanced(_ILCONCERT_INCLUDE_DIR)

if(NOT _ILCONCERT_INCLUDE_DIR)
	if(NOT CPLEX_FIND_QUIETLY)
		message(STATUS "Could not find directory for ilconcert/iloenv.h")
	endif()
else()
    if(NOT CPLEX_FIND_QUIETLY)
        message(STATUS "CPLEX include directory for ilconcert/iloenv.h: ${_ILCONCERT_INCLUDE_DIR}")
    endif()
endif()

if(MSVC)
	# some people still build for 32 bit, but newer cplex versions do not support that...
	if("${CMAKE_GENERATOR}" MATCHES "(Win64|IA64)")
		set(_CPLEX_MSVC_ARCHITECTURE "x64_windows_vs")
	else()
		set(_CPLEX_MSVC_ARCHITECTURE "x86_windows_vs")
	endif()
	
	# make sure to use the right libs
	if("${MSVC_VERSION}" STREQUAL "1600")
		set(_CPLEX_MSVC_VARIANT "${_CPLEX_MSVC_ARCHITECTURE}2010")
	elseif("${MSVC_VERSION}" STREQUAL "1700")
		set(_CPLEX_MSVC_VARIANT "${_CPLEX_MSVC_ARCHITECTURE}2012")
	elseif("${MSVC_VERSION}" STREQUAL "1800")
		set(_CPLEX_MSVC_VARIANT "${_CPLEX_MSVC_ARCHITECTURE}2013")
	elseif("${MSVC_VERSION}" STREQUAL "1900")
		set(_CPLEX_MSVC_VARIANT "${_CPLEX_MSVC_ARCHITECTURE}2015")
	elseif("${MSVC_VERSION}" LESS "2000" AND "${MSVC_VERSION}" GREATER "1909")
		set(_CPLEX_MSVC_VARIANT "${_CPLEX_MSVC_ARCHITECTURE}2017")
	else()
		if(NOT CPLEX_FIND_QUIETLY)
			message(STATUS "This version (MSVC_VERSION: ${MSVC_VERSION}) is not supported!")
		endif()
	endif()
	
	if(NOT CPLEX_FIND_QUIETLY)
		message(STATUS "MSVC variant detected: ${_CPLEX_MSVC_VARIANT}")
	endif()
elseif(WIN32)
	if(NOT CPLEX_FIND_QUIETLY)
		message(STATUS "This compiler is not supported!")
	endif()
endif()

if(_CPLEX_MSVC_VARIANT)
	set(_CPLEX_MSVC_HINTDIR_RELEASE "${CPLEX_ROOT}/cplex/lib/${_CPLEX_MSVC_VARIANT}/stat_mda")
	set(_CONCERT_MSVC_HINTDIR_RELEASE "${CPLEX_ROOT}/concert/lib/${_CPLEX_MSVC_VARIANT}/stat_mda")
	set(_CPLEX_MSVC_HINTDIR_DEBUG "${CPLEX_ROOT}/cplex/lib/${_CPLEX_MSVC_VARIANT}/stat_mdd")
	set(_CONCERT_MSVC_HINTDIR_DEBUG "${CPLEX_ROOT}/concert/lib/${_CPLEX_MSVC_VARIANT}/stat_mdd")
endif()

if(_CPLEX_MSVC_VARIANT)
	find_library(
		_ILOCPLEX_LIBRARY_DEBUG
		NAMES
			ilocplex
		HINTS
			${_CPLEX_MSVC_HINTDIR_DEBUG}
	)
	
	find_library(
		_ILOCPLEX_LIBRARY_RELEASE
		NAMES
			ilocplex
		HINTS
			${_CPLEX_MSVC_HINTDIR_RELEASE}
	)

	if(_ILOCPLEX_LIBRARY_DEBUG)
		set(_ILOCPLEX_LIBRARY "debug" ${_ILOCPLEX_LIBRARY_DEBUG})
	endif()
	
	if(_ILOCPLEX_LIBRARY_RELEASE)
		set(_ILOCPLEX_LIBRARY ${_ILOCPLEX_LIBRARY} "optimized" ${_ILOCPLEX_LIBRARY_RELEASE})
	endif()
else()
	find_library(
		_ILOCPLEX_LIBRARY
		NAMES
			ilocplex
		HINTS
			"${CPLEX_ROOT}/cplex"
			"${CPLEX_ROOT}/cplex/lib"
			"${CPLEX_ROOT}/cplex/lib/x86-64_linux/static_pic"
			"${CPLEX_ROOT}/cplex/lib/x86-64_osx/static_pic"
			"${CPLEX_ROOT}/cplex/lib/x86-64_darwin/static_pic"
			"${CPLEX_ROOT}/cplex/lib/x86-64_sles10_4.1/static_pic"
			"$ENV{LIBRARY_PATH}"
			"$ENV{LD_LIBRARY_PATH}"
	)

	set(_ILOCPLEX_LIBRARY_DEBUG ${_ILOCPLEX_LIBRARY})
	set(_ILOCPLEX_LIBRARY_RELEASE ${_ILOCPLEX_LIBRARY})
endif()
mark_as_advanced(_ILOCPLEX_LIBRARY _ILOCPLEX_LIBRARY_DEBUG _ILOCPLEX_LIBRARY_RELEASE)

if(NOT _ILOCPLEX_LIBRARY)
	if(NOT CPLEX_FIND_QUIETLY)
		message(STATUS "Could not find library ilocplex!")
	endif()
else()
	if(NOT CPLEX_FIND_QUIETLY)
		message(STATUS "CPLEX library ilocplex: ${_ILOCPLEX_LIBRARY}")
	endif()
endif()

if(_CPLEX_MSVC_VARIANT)
	find_library(
		_CPLEX_A_LIBRARY_DEBUG
		NAMES
			cplex
			cplex1271
			cplex1270
		HINTS
			${_CPLEX_MSVC_HINTDIR_DEBUG}
	)

	find_library(
		_CPLEX_A_LIBRARY_RELEASE
		NAMES
			cplex
			cplex1271
			cplex1270
		HINTS
			${_CPLEX_MSVC_HINTDIR_RELEASE}
	)
	
	if(_CPLEX_A_LIBRARY_DEBUG)
		set(_CPLEX_A_LIBRARY "debug" ${_CPLEX_A_LIBRARY_DEBUG})
	endif()
	
	if(_CPLEX_A_LIBRARY_RELEASE)
		set(_CPLEX_A_LIBRARY ${_CPLEX_A_LIBRARY} "optimized" ${_CPLEX_A_LIBRARY_RELEASE})
	endif()
else()
	find_library(
		_CPLEX_A_LIBRARY
		NAMES
			cplex
		HINTS
			"${CPLEX_ROOT}/cplex"
			"${CPLEX_ROOT}/cplex/lib"
			"${CPLEX_ROOT}/cplex/lib/x86-64_linux/static_pic"
			"${CPLEX_ROOT}/cplex/lib/x86-64_osx/static_pic"
			"${CPLEX_ROOT}/cplex/lib/x86-64_darwin/static_pic"
			"${CPLEX_ROOT}/cplex/lib/x86-64_sles10_4.1/static_pic"
			"$ENV{LIBRARY_PATH}"
			"$ENV{LD_LIBRARY_PATH}"
	)

	set(_CPLEX_A_LIBRARY_DEBUG ${_CPLEX_A_LIBRARY})
	set(_CPLEX_A_LIBRARY_RELEASE ${_CPLEX_A_LIBRARY})
endif()
mark_as_advanced(_CPLEX_A_LIBRARY _CPLEX_A_LIBRARY_DEBUG _CPLEX_A_LIBRARY_RELEASE)

if(NOT _CPLEX_A_LIBRARY)
	if(NOT CPLEX_FIND_QUIETLY)
		message(STATUS "Could not find static library cplex!")
	endif()
else()
	if(NOT CPLEX_FIND_QUIETLY)
		message(STATUS "CPLEX static library: ${_CPLEX_A_LIBRARY}")
	endif()
endif()

if(_CPLEX_MSVC_VARIANT)
	find_library(
		_ILCONCERT_LIBRARY_DEBUG
		NAMES
			concert
		HINTS
			${_CONCERT_MSVC_HINTDIR_DEBUG}
	)
	
	find_library(
		_ILCONCERT_LIBRARY_RELEASE
		NAMES
			concert
		HINTS
			${_CONCERT_MSVC_HINTDIR_RELEASE}
	)
	
	if(_ILCONCERT_LIBRARY_DEBUG)
		set(_ILCONCERT_LIBRARY "debug" ${_ILCONCERT_LIBRARY_DEBUG})
	endif()
	
	if(_ILCONCERT_LIBRARY_RELEASE)
		set(_ILCONCERT_LIBRARY ${_ILCONCERT_LIBRARY} "optimized" ${_ILCONCERT_LIBRARY_RELEASE})
	endif()
else()
	find_library(
		_ILCONCERT_LIBRARY
		NAMES
			concert
		HINTS
			"${CPLEX_ROOT}/concert"
			"${CPLEX_ROOT}/concert/lib"
			"${CPLEX_ROOT}/concert/lib/x86-64_linux/static_pic"
			"${CPLEX_ROOT}/concert/lib/x86-64_osx/static_pic"
			"${CPLEX_ROOT}/concert/lib/x86-64_darwin"
			"${CPLEX_ROOT}/concert/lib/x86-64_darwin/static_pic"
			"${CPLEX_ROOT}/concert/lib/x86-64_sles10_4.1/static_pic"
			"$ENV{LIBRARY_PATH}"
			"$ENV{LD_LIBRARY_PATH}"
	)
	set(_ILCONCERT_LIBRARY_DEBUG ${_ILCONCERT_LIBRARY})
	set(_ILCONCERT_LIBRARY_RELEASE ${_ILCONCERT_LIBRARY})
endif()
mark_as_advanced(_ILCONCERT_LIBRARY _ILCONCERT_LIBRARY_RELEASE _ILCONCERT_LIBRARY_DEBUG)

if(NOT _ILCONCERT_LIBRARY)
	if(NOT CPLEX_FIND_QUIETLY)
		message(STATUS "Could not find library concert!")
	endif()
else()
	if(NOT CPLEX_FIND_QUIETLY)
		message(STATUS "CPLEX library concert: ${_ILCONCERT_LIBRARY}")
	endif()
endif()

find_package_handle_standard_args(
	CPLEX
	DEFAULT_MSG
	CPLEX_ROOT
	_ILCPLEX_INCLUDE_DIR
	_ILCONCERT_INCLUDE_DIR
	_ILOCPLEX_LIBRARY
	_ILCONCERT_LIBRARY
	_CPLEX_A_LIBRARY
)

if(CPLEX_FOUND)
    set(CPLEX_LIBRARIES "${_ILOCPLEX_LIBRARY};${_ILCONCERT_LIBRARY};${_CPLEX_A_LIBRARY}")
    set(CPLEX_INCLUDE_DIRS "${_ILCPLEX_INCLUDE_DIR};${_ILCONCERT_INCLUDE_DIR}")

	add_library(algutil::ilconcert UNKNOWN IMPORTED)
	set_target_properties(algutil::ilconcert PROPERTIES
		IMPORTED_LOCATION_DEBUG
			${_ILCONCERT_LIBRARY_DEBUG}
		IMPORTED_LOCATION
			${_ILCONCERT_LIBRARY_RELEASE}
		IMPORTED_LOCATION_RELEASE
			${_ILCONCERT_LIBRARY_RELEASE}
		IMPORTED_LOCATION_RELWITHDEBINFO
			${_ILCONCERT_LIBRARY_RELEASE}
	        IMPORTED_LOCATION_MINSIZEREL
			${_ILCONCERT_LIBRARY_RELEASE}
		INTERFACE_INCLUDE_DIRECTORIES
			"${_ILCONCERT_INCLUDE_DIR}"
	)
    util_imported_link_libraries(algutil::ilconcert algutil::ilcplex)

    add_library(algutil::ilcplex UNKNOWN IMPORTED)
	set_target_properties(algutil::ilcplex PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES
			"${_ILCPLEX_INCLUDE_DIR}"
		IMPORTED_LOCATION_DEBUG
			${_CPLEX_A_LIBRARY_DEBUG}
		IMPORTED_LOCATION
			${_CPLEX_A_LIBRARY_RELEASE}
		IMPORTED_LOCATION_RELEASE
			${_CPLEX_A_LIBRARY_RELEASE}
		IMPORTED_LOCATION_RELWITHDEBINFO
			${_CPLEX_A_LIBRARY_RELEASE}
	        IMPORTED_LOCATION_MINSIZEREL
			${_CPLEX_A_LIBRARY_RELEASE}
	)
	util_imported_link_libraries(algutil::ilcplex Threads::Threads)
	if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		util_imported_link_libraries(algutil::ilcplex "dl")
	endif()

	add_library(algutil::cplex UNKNOWN IMPORTED)
	set_target_properties(algutil::cplex PROPERTIES
		IMPORTED_LOCATION
			${_ILOCPLEX_LIBRARY_RELEASE}
		IMPORTED_LOCATION_DEBUG
			${_ILOCPLEX_LIBRARY_DEBUG}
		IMPORTED_LOCATION_RELEASE
			${_ILOCPLEX_LIBRARY_RELEASE}
		IMPORTED_LOCATION_RELWITHDEBINFO
			${_ILOCPLEX_LIBRARY_RELEASE}
		IMPORTED_LOCATION_MINSIZEREL
		    ${_ILOCPLEX_LIBRARY_RELEASE}
	)
	util_imported_link_libraries(algutil::cplex algutil::ilconcert)
endif()
