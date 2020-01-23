# ----------------------------------------------------------------------------------------------------------------------
# Configuration:
# ----------------------------------------------------------------------------------------------------------------------
SET(AREPA_DEFAULT_CXX_STANDARD 17)

# ----------------------------------------------------------------------------------------------------------------------
# Include:
# ----------------------------------------------------------------------------------------------------------------------
include("${CMAKE_CURRENT_LIST_DIR}/common.cmake")
file(GLOB _lib_scripts "${CMAKE_CURRENT_LIST_DIR}/lib_*.cmake")
foreach(_lib_script ${_lib_scripts})
	include("${_lib_script}")
endforeach()

# ----------------------------------------------------------------------------------------------------------------------
# Functions:
# ----------------------------------------------------------------------------------------------------------------------

# Function:    arepa_target_cxx
# Description:
#
#     Configures a pre-existing target to use the Arepa C++ module setup and layout.
#
#     Layout:
#      * `include` - Exported header files.
#      * `src`     - Private sources.
#
function(arepa_target_cxx module_name)
	# Set the include directories.
	target_include_directories("${module_name}"
		PUBLIC
			$<INSTALL_INTERFACE:include>
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/src
	)

	# Set the source directories.
	file(GLOB "${module_name}_SRC"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
	)
	target_sources("${module_name}" PRIVATE ${${module_name}_SRC})

	# Configure the C++ properties.
	set_target_properties(networking
		PROPERTIES
			LINKER_LANGUAGE CXX
			CXX_STANDARD "${AREPA_DEFAULT_CXX_STANDARD}"
	)
endfunction(arepa_target_cxx)
