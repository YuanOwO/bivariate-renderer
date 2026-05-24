cmake_minimum_required(VERSION 3.16)
project(MyProject)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# 尋找 Flex 和 Bison 工具
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# 設定 include 目錄
# include_directories(include  src ${CMAKE_CURRENT_BINARY_DIR})

# -------------------------
# Bison / Flex 生成
# -------------------------

BISON_TARGET(MyParser
    src/parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.hpp
)

FLEX_TARGET(MyLexer
    src/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cpp
)

ADD_FLEX_BISON_DEPENDENCY(MyLexer MyParser)

# -------------------------
# Source files
# -------------------------
file(GLOB_RECURSE C_SOURCES CONFIGURE_DEPENDS
    src/*.c
)

file(GLOB_RECURSE CXX_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

# -------------------------
# Executable
# -------------------------
add_executable(main
    ${C_SOURCES}
    ${CXX_SOURCES}
    ${BISON_MyParser_OUTPUTS}
    ${FLEX_MyLexer_OUTPUTS}
)

# -------------------------
# Include directories (modern way)
# -------------------------
target_include_directories(main PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

##############################
# 編譯選項
#   cmake -DCMAKE_BUILD_TYPE=Debug ..
#   cmake -DCMAKE_BUILD_TYPE=Release ..
##############################

# # Debug 編譯選項
# set(CMAKE_C_FLAGS_DEBUG "-g -O1 -fsanitize=undefined")
# set(CMAKE_CXX_FLAGS_DEBUG "-g -O1 -fsanitize=undefined")

# # Release 編譯選項
# set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
# set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# -------------------------
# Compile options (modern way)
# -------------------------
target_compile_options(main PRIVATE
    $<$<CONFIG:Debug>:-g -O1 -fsanitize=undefined>
    $<$<CONFIG:Release>:-O2 -DNDEBUG>
)

# optional: link sanitizer if needed
target_link_options(main PRIVATE
    $<$<CONFIG:Debug>:-fsanitize=undefined>
)
