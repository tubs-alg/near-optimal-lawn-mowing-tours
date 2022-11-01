include(FindPackageHandleStandardArgs)

# make GUROBI_ROOT a cached option
if(NOT GUROBI_ROOT)
	set(GUROBI_ROOT "" CACHE STRING "The root directory of your Gurobi installation.")
endif()

# protect from multiple inclusion
if(NOT COMMAND find_gurobi)
	function(find_gurobi_in _DIRECTORY)
		find_path(
			_GUROBI_CPP_INCLUDE_DIR
			NAMES
				"gurobi_c++.h"
			HINTS
				/opt/tmp/gurobi911/linux64/include
				${_DIRECTORY}/include
				${_DIRECTORY}
				${_DIRECTORY}/*/include
				${_DIRECTORY}/*/*/include
		)

		if(NOT _GUROBI_CPP_INCLUDE_DIR)
			return()
		endif()

		find_library(
			_GUROBI_CPP_STATIC_LIB
			NAMES
				"gurobi_c++" "libgurobi_c++.a" "gurobi_c++.lib" "libgurobi_c++.lib"
			HINTS
				/opt/tmp/gurobi911/linux64/bin
				/opt/tmp/gurobi911/linux64/lib
				${_DIRECTORY}/lib
				${_DIRECTORY}/bin
				${_DIRECTORY}/*/lib
				${_DIRECTORY}/*/bin
				${_DIRECTORY}/*/*/lib
				${_DIRECTORY}/*/*/bin
		)

		if(NOT _GUROBI_CPP_STATIC_LIB)
			return()
		endif()

		get_filename_component(_GUROBI_LIB_RP "${_GUROBI_CPP_STATIC_LIB}" REALPATH)
		get_filename_component(_GUROBI_LIBDIR "${_GUROBI_LIB_RP}" DIRECTORY)

		file(GLOB _GUROBI_GLOB_RESULT "${_GUROBI_LIBDIR}/*gurobi*.*")
		foreach(_F IN LISTS _GUROBI_GLOB_RESULT)
			string(REGEX MATCH "^.*/(lib)?gurobi([0-9]+)\\.(a|lib|dll|so|dylib)$" _GUROBI_GLOB_REGEX_MATCHED "${_F}")
			if(_GUROBI_GLOB_REGEX_MATCHED)
				set(_GUROBI_DLL_VERSION ${CMAKE_MATCH_2})
				break()
			endif()
		endforeach()

		find_library(
			_GUROBI_DLL
			NAMES
				"gurobi${_GUROBI_DLL_VERSION}"
			HINTS
				/opt/tmp/gurobi911/linux64/lib
				${_GUROBI_LIBDIR}
		)

		if(NOT _GUROBI_DLL)
			return()
		endif()

		util_add_imported_library(algutil::gurobi_c ${_GUROBI_DLL} ${_GUROBI_CPP_INCLUDE_DIR})
		util_add_imported_library(algutil::gurobi ${_GUROBI_CPP_STATIC_LIB} ${_GUROBI_CPP_INCLUDE_DIR})

		find_library(_GUROBI_LIBM NAMES m libm)
		find_library(_GUROBI_LIBDL NAMES dl libdl)
		find_library(_GUROBI_LIBRT NAMES rt librt)
		find_package(Threads)
		
		util_imported_link_libraries(algutil::gurobi_c Threads::Threads)
		if(_GUROBI_LIBM)
			util_imported_link_libraries(algutil::gurobi_c "${_GUROBI_LIBM}")
		endif()

		if(_GUROBI_LIBDL)
			util_imported_link_libraries(algutil::gurobi_c "${_GUROBI_LIBDL}")
		endif()

		if(_GUROBI_LIBRT)
			util_imported_link_libraries(algutil::gurobi_c "${_GUROBI_LIBRT}")
		endif()

		util_imported_link_libraries(algutil::gurobi algutil::gurobi_c)
	endfunction(find_gurobi_in)

	function(find_gurobi)
		# find Gurobi based on GUROBI_ROOT
		if(GUROBI_ROOT)
			find_gurobi_in("${GUROBI_ROOT}")
			if(TARGET algutil::gurobi)
				return()
			endif()
		endif()

		# find Gurobi based on the gurobi_cl executable on the commandline
		find_program(_GUROBI_EXECUTABLE_BINARY "gurobi_cl")
		if(_GUROBI_EXECUTABLE_BINARY)
			get_filename_component(_GUROBI_REAL_EXECUTABLE_BINARY "${_GUROBI_EXECUTABLE_BINARY}" REALPATH)
			get_filename_component(_GUROBI_EXECUTABLE_DIR "${_GUROBI_REAL_EXECUTABLE_BINARY}" DIRECTORY)
			get_filename_component(_GUROBI_BASE_DIR "${_GUROBI_EXECUTABLE_DIR}" DIRECTORY)

			find_gurobi_in("${_GUROBI_BASE_DIR}")
			if(TARGET algutil::gurobi)
				return()
			endif()
		endif()

		# find Gurobi using static guesses
		set(_GUROBI_GUESSES "/Library;C:/Program Files;/usr/local;/opt;/opt/local;/usr;/opt/tmp/gurobi911/")
		
		foreach(_GUROBI_GUESS IN LISTS _GUROBI_GUESSES)
			if(IS_DIRECTORY ${_GUROBI_GUESS})
				if(NOT FIND_GUROBI_QUIETLY)
					message(STATUS "Searching '${_GUROBI_GUESS}' for Gurobi (this may take a while)...")
				endif()
				
				# search for a file that we are sure exists in every recent gurobi installation
				FILE(GLOB_RECURSE _GUROBI_FILES "${_GUROBI_GUESS}/*gurobi.jar")
				
				foreach(_F IN LISTS _GUROBI_FILES)
					if(EXISTS ${_F} AND NOT IS_DIRECTORY ${_F})
						get_filename_component(_GUROBI_JAR_DIR "${_F}" DIRECTORY)
						get_filename_component(_GUROBI_JAR_PAR "${_GUROBI_JAR_DIR}" DIRECTORY)
						find_gurobi_in("${_GUROBI_JAR_PAR}")
						if(TARGET algutil::gurobi)
							return()
						endif()
					endif()
				endforeach()
			endif()
		endforeach()

		# otherwise, fail
		find_package_handle_standard_args(GUROBI DEFAULT_MSG _GUROBI_CPP_STATIC_LIB _GUROBI_DLL _GUROBI_CPP_INCLUDE_DIR)
	endfunction(find_gurobi)

	find_gurobi()
endif()

