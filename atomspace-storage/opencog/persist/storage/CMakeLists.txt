
# --------------------------------------------------

OPENCOG_GEN_CXX_ATOMTYPES(storage_types.script
	storage_types.h
	storage_types.definitions
	storage_types.inheritance)

OPENCOG_GEN_SCM_ATOMTYPES(storage_types.script storage_types.scm)
OPENCOG_GEN_PYTHON_ATOMTYPES(storage_types.script storage_types.pyx)

ADD_CUSTOM_TARGET(storage_types
	DEPENDS storage_types.h storage_types.scm storage_types.pyx)

# The atom_types.h file is written to the build directory
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

ADD_LIBRARY(storage-types SHARED
	storage_types_init.cc
)

# Without this, parallel make will race and crap up the generated files.
ADD_DEPENDENCIES(storage-types storage_types)

TARGET_LINK_LIBRARIES(storage-types
	${ATOMSPACE_atomtypes_LIBRARY}
)

INSTALL (TARGETS storage-types
	EXPORT AtomSpaceStorageTargets
	LIBRARY DESTINATION "lib${LIB_DIR_SUFFIX}/opencog"
)

INSTALL (FILES
	${CMAKE_CURRENT_BINARY_DIR}/storage_types.h
	DESTINATION "include/opencog/persist/storage"
)

# Install the auto-generated atom types as well
ADD_GUILE_MODULE (FILES
	${CMAKE_CURRENT_BINARY_DIR}/storage_types.scm
	MODULE_DESTINATION "${GUILE_SITE_DIR}/opencog/persist/types"
	DEPENDS storage_types
)
ADD_GUILE_EXTENSION(SCM_CONFIG storage-types "opencog-ext-path-storage-types")
