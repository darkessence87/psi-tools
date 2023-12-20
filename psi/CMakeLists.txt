include_directories(
    ${psi_logger_dir}/psi/include
    include
    src
)

link_directories(
    ${psi_logger_dir}/build/bin/${CMAKE_BUILD_TYPE}
    ${psi_shared_dir}/build/bin/${CMAKE_BUILD_TYPE}
)

set (SOURCES
    src/psi/tools/crypt/Encryptor_x25519.cpp
    src/psi/tools/crypt/EncryptorAes.cpp
    src/psi/tools/crypt/EncryptorBase64.cpp
    src/psi/tools/ByteBuffer.cpp
    src/psi/tools/Encryptor.cpp
    src/psi/tools/Tools.cpp
)

add_library(psi-tools ${SOURCES})

set(TEST_SRC
    tests/crypt/Encryptor_x25519_Tests.cpp
    tests/crypt/EncryptorAesTests.cpp
    tests/crypt/EncryptorBase64Tests.cpp
    tests/ByteBufferTests.cpp
    tests/EncryptorTests.cpp
    tests/ToolsTests.cpp
)
psi_make_tests("Tools" "${TEST_SRC}" "psi-tools;psi-logger;psi-shared")

psi_make_examples("1_ToolsExamples" "examples/1_ToolsExamples.cpp" "psi-tools;psi-logger;psi-shared")
psi_make_examples("2_ByteBufferExamples" "examples/2_ByteBufferExamples.cpp" "psi-tools;psi-logger;psi-shared")
psi_make_examples("3_EncryptorExamples" "examples/3_EncryptorExamples.cpp" "psi-tools;psi-logger;psi-shared")