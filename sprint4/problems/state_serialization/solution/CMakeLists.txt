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
		src/model/geom.h
		src/model/movement.h
		src/model/loot.h
		src/model/road.cpp
		src/model/road.h
		src/model/building.cpp
		src/model/building.h
		src/model/office.cpp
		src/model/office.h
		src/model/map.cpp
		src/model/map.h
		src/model/dog.cpp
		src/model/dog.h
		src/model/game_session.cpp
		src/model/game_session.h
		src/model/game.cpp
		src/model/game.h
		src/model/collision_detector.cpp
		src/model/collision_detector.h
		src/model/item_gatherer.h
		src/model/tagged.h
		src/sdk.h
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
        src/serialize.h
)

set(APPLICATION
		src/app/application.cpp
		src/app/application.h
		src/app/players.cpp
		src/app/players.h
		src/app/application_listener.h
)

set(INFRASTRUCURE
		src/infrastructure/serializing_listener.h
		src/infrastructure/serializing_listener.cpp
)

set(MODEL_SERIALIZE
		src/model_serialize/model_serialize.h
		src/model_serialize/ser_dog.h
		src/model_serialize/ser_game_session.h
		src/model_serialize/ser_geom.h
		src/model_serialize/ser_loot.h
		src/model_serialize/ser_game.h
)

set(APP_SERIALIZE
		src/app_serialize/ser_players.h
		src/app_serialize/ser_app.h
)

set(SERIALIZE
	src/serialize.h
	${MODEL_SERIALIZE}
	${APP_SERIALIZE}
)

set(TESTS
		tests/collision_detector_tests.cpp
		tests/loot_generator_tests.cpp
		tests/model_tests.cpp
		tests/state_serialization_tests.cpp
)

include(CTest)
include(${CONAN_BUILD_DIRS_CATCH2}/Catch.cmake)

set(BOOST_LIB CONAN_PKG::boost)
set(ZLIB_LIB Threads::Threads)
set(CATCH2_LIB CONAN_PKG::catch2)


set(MODEL_LIB ModelLib)
add_library(${MODEL_LIB} STATIC
		${LOOT}
		${MODEL}
		${MODEL_SERIALIZE}
)

target_include_directories(${MODEL_LIB} PUBLIC ${BOOST_LIB})
target_link_libraries(${MODEL_LIB} PUBLIC ${BOOST_LIB} ${ZLIB_LIB})

add_executable(${PROJECT_NAME}
		src/main.cpp
		${PARSE}
		${UTIL}
		${HTTP_SERVER}
		${JSON}
		${HANDLER}
		${LOGGER}
		${APPLICATION}
		${SERIALIZE}
		${INFRASTRUCURE}
)

set(GAME_SERVER_TESTS game_server_tests)
add_executable(${GAME_SERVER_TESTS}
		${TESTS}
		${JSON}
		${APPLICATION}
		${SERIALIZE}
		${LOGGER}
		${INFRASTRUCURE}
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${MODEL_LIB})
target_link_libraries(${GAME_SERVER_TESTS} PRIVATE ${CATCH2_LIB} ${MODEL_LIB})
catch_discover_tests(${GAME_SERVER_TESTS})