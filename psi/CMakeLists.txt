include_directories(
    include
    src
)

set (SOURCES
    src/psi/tools/crypt/aes_gcm.cpp
    src/psi/tools/crypt/aes.cpp
    src/psi/tools/crypt/base64.cpp
    src/psi/tools/crypt/sha.cpp
    src/psi/tools/crypt/x25519.cpp
    src/psi/tools/BigInteger.cpp
    src/psi/tools/BitSet.cpp
    src/psi/tools/ByteBuffer.cpp
    src/psi/tools/HttpParser.cpp
    src/psi/tools/Encryptor.cpp
    src/psi/tools/Tools.cpp
)

set (target_lib "psi-tools")

add_library(${target_lib} ${SOURCES})
psi_config_target(${target_lib} FALSE)

set(TEST_SRC
    tests/crypt/aes_gcm_Tests.cpp
    tests/crypt/aes_Tests.cpp
    tests/crypt/base64_Tests.cpp
    tests/crypt/sha_Tests.cpp
    tests/crypt/x25519_Tests.cpp
    tests/BigIntegerTests.cpp
    tests/BitSetTests.cpp
    tests/ByteBufferTests.cpp
    tests/EncryptorTests.cpp
    tests/HttpParserTests.cpp
    tests/ToolsTests.cpp
)
psi_make_tests("Tools" "${TEST_SRC}" "${target_lib}")

psi_make_examples("1_ToolsExamples" "examples/1_ToolsExamples.cpp" "${target_lib}")
psi_make_examples("2_ByteBufferExamples" "examples/2_ByteBufferExamples.cpp" "${target_lib}")
psi_make_examples("3_EncryptorExamples" "examples/3_EncryptorExamples.cpp" "${target_lib}")
psi_make_examples("4_BigIntegerExamples" "examples/4_BigIntegerExamples.cpp" "${target_lib}")