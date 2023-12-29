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
    src/psi/tools/crypt/aes_gcm.cpp
    src/psi/tools/crypt/aes.cpp
    src/psi/tools/crypt/base64.cpp
    src/psi/tools/crypt/sha.cpp
    src/psi/tools/crypt/x25519.cpp
    src/psi/tools/BigInteger.cpp
    src/psi/tools/ByteBuffer.cpp
    src/psi/tools/Encryptor.cpp
    src/psi/tools/Tools.cpp
)

add_library(psi-tools ${SOURCES})

set(TEST_SRC
    tests/crypt/aes_gcm_Tests.cpp
    tests/crypt/aes_Tests.cpp
    tests/crypt/base64_Tests.cpp
    tests/crypt/sha_Tests.cpp
    tests/crypt/x25519_Tests.cpp
    tests/BigIntegerTests.cpp
    tests/ByteBufferTests.cpp
    tests/EncryptorTests.cpp
    tests/ToolsTests.cpp
)
psi_make_tests("Tools" "${TEST_SRC}" "psi-tools;psi-logger;psi-shared")

psi_make_examples("1_ToolsExamples" "examples/1_ToolsExamples.cpp" "psi-tools;psi-logger;psi-shared")
psi_make_examples("2_ByteBufferExamples" "examples/2_ByteBufferExamples.cpp" "psi-tools;psi-logger;psi-shared")
psi_make_examples("3_EncryptorExamples" "examples/3_EncryptorExamples.cpp" "psi-tools;psi-logger;psi-shared")