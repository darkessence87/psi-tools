include_directories(
    ${psi_logger_dir}/psi/include
    include
)

link_directories(
    ${psi_logger_dir}/build/bin/${CMAKE_BUILD_TYPE}
    ${psi_shared_dir}/build/bin/${CMAKE_BUILD_TYPE}
)

set (SOURCES
    src/psi/tools/ByteBuffer.cpp
    src/psi/tools/Encryptor.cpp
    src/psi/tools/Tools.cpp
    src/psi/tools/x25519.cpp
)

add_library(psi-tools ${SOURCES})

set(TEST_SRC
    tests/ByteBufferTests.cpp
    tests/EncryptorTests.cpp
    tests/ToolsTests.cpp
)
psi_make_tests("Tools" "${TEST_SRC}" "psi-tools;psi-logger;psi-shared")

psi_make_examples("1_ToolsExamples" "examples/1_ToolsExamples.cpp" "psi-tools;psi-logger;psi-shared")
psi_make_examples("2_ByteBufferExamples" "examples/2_ByteBufferExamples.cpp" "psi-tools;psi-logger;psi-shared")
psi_make_examples("3_EncryptorExamples" "examples/3_EncryptorExamples.cpp" "psi-tools;psi-logger;psi-shared")