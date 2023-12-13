include_directories(
    include
)

set (SOURCES
    src/psi/tools/ByteBuffer.cpp
    src/psi/tools/Encryptor.cpp
    src/psi/tools/Tools.cpp
)

add_library(psi-tools ${SOURCES})

set(TEST_SRC
    tests/ByteBufferTests.cpp
    tests/EncryptorTests.cpp
    tests/ToolsTests.cpp
)
psi_make_tests("Tools" "${TEST_SRC}" "psi-tools")

psi_make_examples("1_ToolsExamples" "examples/1_ToolsExamples.cpp" "psi-tools")
psi_make_examples("2_ByteBufferExamples" "examples/2_ByteBufferExamples.cpp" "psi-tools")
psi_make_examples("3_EncryptorExamples" "examples/3_EncryptorExamples.cpp" "psi-tools")