include_directories(
    include
)

set (SOURCES
    src/psi/tools/Tools.cpp
)

add_library(psi-tools ${SOURCES})

# set(TEST_SRC
#     tests/
# )
# psi_make_tests("Tools" "${TEST_SRC}" "")

set(EXAMPLE_SRC
    examples/EntryPoint.cpp
)
psi_make_examples("1.0_Simple_Tools" "${EXAMPLE_SRC}" "psi-tools")