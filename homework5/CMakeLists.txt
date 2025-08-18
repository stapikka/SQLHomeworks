cmake_minimum_required(VERSION 3.22)
project(SQLCMake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PostgreSQL_ROOT "C:/Program Files/PostgreSQL/17")
# Ищем libpq (клиентскую библиотеку PostgreSQL)
find_package(PostgreSQL REQUIRED)

# Добавляем libpqxx как подпроект
add_subdirectory(C:/Users/balun/LIBRARYS/libpqxx libpqxx-build)

add_executable(namename source.cpp)


# Подключаем обе библиотеки
target_link_libraries(namename PRIVATE pqxx)
target_include_directories(namename PRIVATE ${PostgreSQL_INCLUDE_DIRS})
target_link_directories(namename PRIVATE ${PostgreSQL_LIBRARY_DIRS})