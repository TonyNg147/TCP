cmake_minimum_required(VERSION 3.16)

project(EventDriven)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(HEADERS
    Broker/EventBroker.h
    Collections/DispatcherCollection.h
    Collections/EventSourceCollection.h
    EventSource/FDEventSource.h
    Dispatcher/FDDispatcher.h
    Dispatcher/Dispatcher.h
    EventListener/BaseEventListener.h
    EventSource/EventSourceBase.h
    SystemType/BaseEvents.h
    SystemType/DispatcherKind.h
    SystemType/EventSourceKind.h
    SystemType/FDEvents.h
    Supports/Streamable.h
)

set(SOURCES
    Collections/DispatcherCollection.cpp
    Collections/EventSourceCollection.cpp
    Dispatcher/FDDispatcher.cpp
    EventSource/FDEventSource.cpp
)


add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE -lpthread)

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/Broker
    ${PROJECT_SOURCE_DIR}/Collections
    ${PROJECT_SOURCE_DIR}/Dispatcher
    ${PROJECT_SOURCE_DIR}/EventListener
    ${PROJECT_SOURCE_DIR}/EventSource
    ${PROJECT_SOURCE_DIR}/SystemType
    ${PROJECT_SOURCE_DIR}/Supports
)


if(DEFINED TEST)
    message("Enable testing.")
    enable_testing()
    add_subdirectory(tests)
endif()






