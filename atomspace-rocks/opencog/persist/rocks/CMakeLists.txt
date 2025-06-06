#
# Build the RocksDB driver for the AtomSpace
#

ADD_LIBRARY (persist-rocks SHARED
	RocksDAG.cc
	RocksFrame.cc
	RocksIO.cc
	RocksStorage.cc
	RocksPersistSCM.cc
)

TARGET_LINK_LIBRARIES(persist-rocks
	${ATOMSPACE_STORAGE_LIBRARIES}
	${ATOMSPACE_LIBRARIES}
	rocksdb
)

ADD_GUILE_EXTENSION(SCM_CONFIG persist-rocks
	"opencog-ext-path-persist-rocks")

# The EXPORT is need to autogenerate CMake boilerplate in the lib
# directory that lets other packages FIND_PACKAGE(AtomSpaceRocks)
INSTALL (TARGETS persist-rocks
	EXPORT AtomSpaceRocksTargets
	DESTINATION "lib${LIB_DIR_SUFFIX}/opencog"
)

INSTALL (FILES
	RocksStorage.h
	DESTINATION "include/opencog/persist/rocks"
)
