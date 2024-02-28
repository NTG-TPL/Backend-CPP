cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

get_property(importTargets DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY IMPORTED_TARGETS)
message(STATUS "${importTargets}")

find_package(Boost 1.78.0 REQUIRED)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(PARSE
	src/parse/parse.h
)

set(UTIL
		src/util/tagged.h
		src/util/util.cpp
		src/util/util.h
)

set(LOOT
	src/loot_generator/loot_generator.h
	src/loot_generator/loot_generator.cpp
)

set(HTTP_SERVER
		src/http_server/http_server.cpp
		src/http_server/http_server.h
)

set(JSON
		src/json/boost_json.cpp
		src/json/json_loader.h
		src/json/json_loader.cpp
		src/json/tag_invoke.cpp
		src/json/tag_invoke.h
)

set(MODEL
		src/model/model.h
		src/model/model.cpp
)

set(HANDLER
		src/request_handler/request_handler.h
		src/request_handler/api_handler.cpp
		src/request_handler/api_handler.h
		src/request_handler/file_handler.cpp
		src/request_handler/file_handler.h
		src/request_handler/content_type.h
		src/request_handler/error_response.h
		src/request_handler/endpoint.h
		src/request_handler/make_response.cpp
		src/request_handler/make_response.h
		src/request_handler/ticker.h
)

set(LOGGER
		src/logger/logger.cpp
		src/logger/logger.h
)

set(APPLICATION
		src/app/application.cpp
		src/app/application.h
		src/app/players.cpp
		src/app/players.h
)

add_executable(game_server
		src/main.cpp
		src/sdk.h
		${PARSE}
		${UTIL}
		${LOOT}
		${HTTP_SERVER}
		${JSON}
		${MODEL}
		${HANDLER}
		${LOGGER}
		${APPLICATION}
)

target_include_directories(${PROJECT_NAME} PRIVATE CONAN_PKG::boost)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads CONAN_PKG::boost)

set(GAME_SERVER_TESTS game_server_tests)

set(TESTS
		tests/loot_generator_tests.cpp
		tests/model_tests.cpp
)

add_executable(${GAME_SERVER_TESTS}
		${TESTS}
		${MODEL}
		${LOOT}
		src/util/tagged.h
)
target_include_directories(${GAME_SERVER_TESTS} PRIVATE  CONAN_PKG::boost)
target_link_libraries(${GAME_SERVER_TESTS} PRIVATE CONAN_PKG::catch2 Threads::Threads CONAN_PKG::boost)