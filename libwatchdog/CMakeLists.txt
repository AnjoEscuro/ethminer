set(SOURCES
    watchdog.h watchdog.cpp
)

add_library(watchdog ${SOURCES})
target_link_libraries(watchdog PRIVATE ethcore devcore)
target_include_directories(watchdog PRIVATE ..)
