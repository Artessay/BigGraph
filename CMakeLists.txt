cmake_minimum_required(VERSION 3.12)
project(MyGraphDB)

# Add your source files here
set(SOURCES
    driver.cpp
    Node.cpp
    PProcedure.cpp
    Value.cpp
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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -g")

# Create the executable
add_executable(driver ${SOURCES})

# Link the libraries
# target_link_libraries(MyProject ${LIBRARIES})