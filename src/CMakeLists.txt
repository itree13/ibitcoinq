cmake_minimum_required (VERSION 3.8)

file(GLOB_RECURSE DIR_SRC
    ${CMAKE_CURRENT_LIST_DIR}/*.h
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/**/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/**/*.h
    ${CMAKE_CURRENT_LIST_DIR}/**/*.hpp
    ${CMAKE_CURRENT_LIST_DIR}/**/*.cpp)

message(${CMAKE_CURRENT_LIST_DIR})
if (MSVC)
#	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") 
endif (MSVC)

add_executable(ibitcoinq
	${DIR_SRC})

	
target_link_libraries(ibitcoinq PUBLIC libssl32MT$<$<CONFIG:Debug>:d> libcrypto32MT$<$<CONFIG:Debug>:d> Crypt32)
install(TARGETS ibitcoinq RUNTIME DESTINATION ibitcoinq)
