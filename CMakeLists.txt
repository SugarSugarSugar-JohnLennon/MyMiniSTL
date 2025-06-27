cmake_minimum_required(VERSION 3.10)
project(MyMiniSTL)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 测试源文件列表
set(TEST_TYPE_TRAITS_SRC test/test_type_traits.cpp)
set(TEST_EXCEPTDEF_SRC test/test_exceptdef.cpp)
set(TEST_ITERATOR_SRC test/test_iterator.cpp)
set(TEST_ALLOCATOR_SRC test/test_allocator.cpp)
set(TEST_UTILITY_SRC test/test_utility.cpp)
set(TEST_VECTOR_SRC test/test_vector.cpp)

# 指定输出目录
set(TEST_TYPE_TRAITS_BIN ${CMAKE_SOURCE_DIR}/test/bin/test_type_traits)
set(TEST_EXCEPTDEF_BIN ${CMAKE_SOURCE_DIR}/test/bin/test_exceptdef)
set(TEST_ITERATOR_BIN ${CMAKE_SOURCE_DIR}/test/bin/test_iterator)
set(TEST_ALLOCATOR_BIN ${CMAKE_SOURCE_DIR}/test/bin/test_allocator)
set(TEST_UTILITY_BIN ${CMAKE_SOURCE_DIR}/test/bin/test_utility)
set(TEST_VECTOR_BIN ${CMAKE_SOURCE_DIR}/test/bin/test_vector)
file(MAKE_DIRECTORY ${TEST_TYPE_TRAITS_BIN})
file(MAKE_DIRECTORY ${TEST_EXCEPTDEF_BIN})
file(MAKE_DIRECTORY ${TEST_ITERATOR_BIN})
file(MAKE_DIRECTORY ${TEST_ALLOCATOR_BIN})
file(MAKE_DIRECTORY ${TEST_UTILITY_BIN})
file(MAKE_DIRECTORY ${TEST_VECTOR_BIN})

# type_traits 测试
add_executable(test_type_traits ${TEST_TYPE_TRAITS_SRC})
set_target_properties(test_type_traits PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${TEST_TYPE_TRAITS_BIN}
)
target_include_directories(test_type_traits PRIVATE .)

# exceptdef 测试
add_executable(test_exceptdef ${TEST_EXCEPTDEF_SRC})
set_target_properties(test_exceptdef PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${TEST_EXCEPTDEF_BIN}
)
target_include_directories(test_exceptdef PRIVATE .)

# iterator 测试
add_executable(test_iterator ${TEST_ITERATOR_SRC})
set_target_properties(test_iterator PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${TEST_ITERATOR_BIN}
)
target_include_directories(test_iterator PRIVATE .)

# allocator 测试
add_executable(test_allocator ${TEST_ALLOCATOR_SRC})
set_target_properties(test_allocator PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${TEST_ALLOCATOR_BIN}
)
target_include_directories(test_allocator PRIVATE .)

# utility 测试
add_executable(test_utility ${TEST_UTILITY_SRC})
set_target_properties(test_utility PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${TEST_UTILITY_BIN}
)
target_include_directories(test_utility PRIVATE .)

# vector 测试
add_executable(test_vector ${TEST_VECTOR_SRC})
set_target_properties(test_vector PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${TEST_VECTOR_BIN}
)
target_include_directories(test_vector PRIVATE .) 