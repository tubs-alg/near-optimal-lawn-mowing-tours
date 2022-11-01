find_package(Git)
execute_process(COMMAND "${GIT_EXECUTABLE}" "rev-parse" "HEAD" WORKING_DIRECTORY "${GET_GIT_REPOSITORY_PATH}" RESULT_VARIABLE UTIL_GIT_RESULT OUTPUT_VARIABLE UTIL_GIT_HEAD_ID)

file(MAKE_DIRECTORY "${GET_GIT_OUTPUT_DIR}/algutil")

if("${UTIL_GIT_RESULT}" STREQUAL "0")
	string(REPLACE "\n" "" UTIL_GIT_HEAD_ID "${UTIL_GIT_HEAD_ID}")
	file(WRITE "${GET_GIT_OUTPUT_DIR}/git_revision_gen.h" "#ifndef ALGUTIL_GIT_REVISION\n#define ALGUTIL_GIT_REVISION \"${UTIL_GIT_HEAD_ID}\"\n#endif\n")
else()
	message(WARNING "Could not get SHA ID of GIT HEAD!")
	file(WRITE "${GET_GIT_OUTPUT_DIR}/git_revision_gen.h" "#ifndef ALGUTIL_GIT_REVISION\n#define ALGUTIL_GIT_REVISION (nullptr)\n#endif\n")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${GET_GIT_OUTPUT_DIR}/git_revision_gen.h" "${GET_GIT_OUTPUT_DIR}/algutil/git_revision.h")
