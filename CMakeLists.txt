ADD_EXECUTABLE(client.out client.cpp file.cpp)
    SET_TARGET_PROPERTIES(client.out PROPERTIES
            COMPILE_FLAGS "-D_CLIENT")

ADD_EXECUTABLE(server.out server.cpp file.cpp)
    SET_TARGET_PROPERTIES(client.out PROPERTIES
            COMPILE_FLAGS "-D_SERVER")
