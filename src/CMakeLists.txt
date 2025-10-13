add_library(bmp bmp/bmp.c)

target_include_directories(bmp PUBLIC "${CMAKE_SOURCE_DIR}/include")

add_executable(converter converter/converter.c)
add_executable(comparer comparer/comparer.c)

target_link_libraries(converter PRIVATE bmp)
target_link_libraries(comparer PRIVATE bmp)

target_compile_options(bmp PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_compile_options(comparer PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_compile_options(converter PRIVATE -Wall -Wextra -Wpedantic -Werror)

add_custom_target(
    testsCompConv ALL
    COMMENT "\n\n"
    COMMAND sh ${CMAKE_SOURCE_DIR}/scripts/tests.sh
)
