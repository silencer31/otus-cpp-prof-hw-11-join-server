cmake_minimum_required(VERSION 3.5)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VERSION 0.0.${PATCH_VERSION})

project(join_server VERSION ${PROJECT_VERSION})

find_package(Boost COMPONENTS REQUIRED system filesystem)
if(NOT Boost_FOUND)
	message(SEND_ERROR "Failed to find boost")
	return()
endif()

find_package(Threads)

if(WIN32)
# see https://www.sqlite.org/howtocompile.html
    include(FetchContent)
    FetchContent_Declare(sqlite3 URL "https://www.sqlite.org/snapshot/sqlite-snapshot-202401231504.tar.gz")
    FetchContent_MakeAvailable(sqlite3)

    if (NOT sqlite3_POPULATED)
        FetchContent_Populate(sqlite3)
    endif()
else()
    find_package(SQLite3 REQUIRED)
endif()

include_directories(${Boost_INCLUDE_DIR})

add_definitions(-DBOOST_ALL_NO_LIB) # Without this string boost filesystem will not compile.

configure_file(version.h.in version.h)

#Main target
if(WIN32) 
add_executable(join_server
	main.cpp
	parser/command_parser.cpp
	notifier/session_notifier.cpp
	network/server/join_server.cpp
	network/session/client_session.cpp
	database/database_manager.cpp
	database/sql/sql_manager.cpp
	collectors/request_collector.cpp
	collectors/result_collector.cpp
	"${sqlite3_SOURCE_DIR}/sqlite3.c"
)
else()
add_executable(join_server
	main.cpp
	parser/command_parser.cpp
	notifier/session_notifier.cpp
	network/server/join_server.cpp
	network/session/client_session.cpp
	database/database_manager.cpp
	database/sql/sql_manager.cpp
	collectors/request_collector.cpp
	collectors/result_collector.cpp
)
endif()

add_executable(join_client
	network/client/join_client.cpp
)

add_library(lib_join_server lib.cpp)

set_target_properties(
    join_server
	join_client
    lib_join_server
    PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(join_server 
	PUBLIC "${sqlite3_SOURCE_DIR}"	
)

target_include_directories(lib_join_server 
    PRIVATE "${CMAKE_BINARY_DIR}"
)

if(WIN32)
target_link_libraries(join_server PRIVATE
	${Boost_LIBRARIES}
    lib_join_server
)
else()
target_link_libraries(join_server PRIVATE
	sqlite3
	${Boost_LIBRARIES}
    lib_join_server
)
endif()

target_link_libraries(join_client PRIVATE
    ${Boost_LIBRARIES}
)

if (MSVC)
    target_compile_options(join_server PRIVATE
        /W4
    )
	target_compile_options(join_client PRIVATE
        /W4
    )
    target_compile_options(lib_join_server PRIVATE
        /W4
    )
else ()
    target_compile_options(join_server PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
	target_compile_options(join_client PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    target_compile_options(lib_join_server PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()

install(TARGETS join_server RUNTIME DESTINATION bin)
install(TARGETS join_client RUNTIME DESTINATION bin)
install(TARGETS lib_join_server RUNTIME DESTINATION lib)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)

enable_testing()