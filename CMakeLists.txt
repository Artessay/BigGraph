cmake_minimum_required(VERSION 3.12)
project(MyGraphDB)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add your source files here
set(SOURCES
    driver.cpp
    Node.cpp
    PProcedure.cpp
    Value.cpp
    Graph.cpp
)

# Add any additional include directories here
# include_directories(include)

# Add any additional library directories here
# link_directories(lib)

# Add any additional libraries here
# set(LIBRARIES
#     mylib
# )

# Add any additional compiler flags here
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O0 -g")

# Create the executable
add_executable(driver ${SOURCES})

# Link the libraries
# target_link_libraries(MyProject ${LIBRARIES})