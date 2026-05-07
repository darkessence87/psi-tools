# psi-tools

A cross-platform C++ utility library providing byte-buffer management, cryptographic operations, big-integer arithmetic, HTTP chunked-message parsing, and general-purpose string/time helpers.

**Supported platforms:** Windows, Linux, macOS  
**Requires:** C++20, CMake 3.16+

---

## Components

### [BigInteger](psi/include/psi/tools/BigInteger.h)

An arbitrary-precision unsigned integer backed by a `std::vector<uint64_t>`.  
Maximum representable value: $2^{64 \cdot \text{max}(\texttt{uint64\_t})}$ bits.

Supported operations: `+`, `+=`, `-`, `-=`, indexed `add`/`substract`, `toString()`.

```cpp
#include "psi/tools/BigInteger.h"
using namespace psi::tools;

BigInteger a(42u);
BigInteger b(std::vector<uint64_t>{1u, 0u}); // multi-word value
BigInteger c = a + b;
std::cout << c.toString();  // "higher_bit ... lower_bit "
```

---

### [BitSet](psi/include/psi/tools/BitSet.h)

A dynamic bit-set supporting up to `max(size_t)` bits.

```cpp
#include "psi/tools/BitSet.h"
using namespace psi::tools;

BitSet bs(16);
bs.set(3);
bs.set(7);
bs.inverse();
std::cout << bs.toString();  // string of '0'/'1' characters
```

Key methods: `size()`, `set(n, bool)`, `test(n)`, `inverse()`, `reverse()`, `toString()`.

---

### [ByteBuffer](psi/include/psi/tools/ByteBuffer.h)

A managed, cursor-based byte buffer.  
Internally owns a `uint8_t*` allocation; tracks independent read and write indices.

**Construction:**
```cpp
#include "psi/tools/ByteBuffer.h"
using namespace psi::tools;

ByteBuffer buf(255);                        // allocate 255 bytes
buf.writeString("Hello world!");
buf.writeHexString("aabbccddeeff");         // raw hex → bytes
buf.write(uint64_t(1024));                  // any trivially-copyable type
uint16_t arr[4]{1, 2, 3, 4};
buf.writeArray(arr, 4);

std::cout << buf.asHexStringFormatted();    // "[ 48 65 6c 6c 6f ... ]"
```

**Reading:**
```cpp
ByteBuffer src("deadbeef", true);           // hex string constructor
uint16_t v{};
src.read(v);                                // reads 2 bytes, advances readIndex

std::string line;
src.readLine(line);                         // reads until default delimiters
```

**Conversion helpers:** `asHexString()`, `asHexStringFormatted()`, `asString()`, `asVector()`, `asHash()`, `asArray<T, N>()`.  
**Cursor control:** `reset()`, `resetRead()`, `resetWrite()`, `clear()`, `resize(n)`, `skipRead(n)`, `skipWrite(n)`.  
**Metrics:** `size()` (allocated), `length()` (bytes written), `remainingLength()` (bytes readable).

---

### [Encryptor](psi/include/psi/tools/Encryptor.h)

A static facade over all cryptographic primitives. All inputs and outputs are `ByteBuffer`.

| Method | Description |
|---|---|
| `encryptBase64(data)` | Base64-encode |
| `decryptBase64(data)` | Base64-decode |
| `encryptAes128(data, key)` | AES-128 ECB encrypt |
| `decryptAes128(data, key)` | AES-128 ECB decrypt |
| `encryptAes128Gcm(data, key, iv, tag, add)` | AES-128-GCM encrypt, fills `tag` |
| `decryptAes128Gcm(data, key, iv, tag, add)` | AES-128-GCM decrypt, verifies `tag` |
| `encryptAes256(data, key)` | AES-256 ECB encrypt |
| `decryptAes256(data, key)` | AES-256 ECB decrypt |
| `sha256(data)` | SHA-256 hash |
| `hmac256(key, data)` | HMAC-SHA-256 |
| `hkdf256(key, seed, info, len)` | HKDF extract+expand |
| `hkdf256Expand(prk, info, len)` | HKDF expand only |
| `hkdf256ExpandLabel(prk, label, hash, len)` | HKDF expand with label |
| `generateSessionKey()` | 32-byte cryptographically random key |
| `x25519_generate_keypair(pub, priv)` | Random X25519 key pair |
| `x25519_scalarmult_base(priv)` | Derive public key from private key |
| `x25519_scalarmult(priv, pub)` | Derive shared secret |

```cpp
#include "psi/tools/Encryptor.h"
using namespace psi::tools;

const auto key  = Encryptor::generateSessionKey();
ByteBuffer msg(512);
msg.writeString("secret message");

const auto enc  = Encryptor::encryptAes256(msg, key);
const auto dec  = Encryptor::decryptAes256(enc, key);
std::cout << dec.asString();
```

---

### [HttpParser](psi/include/psi/tools/HttpParser.h)

Parses HTTP/1.1 chunked-transfer-encoding responses, including fragmented delivery.

```cpp
#include "psi/tools/HttpParser.h"
using namespace psi::tools;

ByteBuffer rawMsg = /* data received from socket */;
std::string header;
std::map<std::string, std::string> meta;
std::ostringstream body;
size_t remaining = HttpParser::MAX_MSG_LENGTH;

bool done = HttpParser::parseHttpChunkedMessage(rawMsg, header, meta, remaining, body);
// done == true  → last chunk (size 0) was received
// done == false → more fragments expected; call parseHttpFragment() for each
```

Key methods:
- `parseHttpChunkedMessage(msg, header, meta, remainingDataSz, data)` — parses the initial message with headers
- `parseHttpFragment(fragment, remainingSz, data)` — parses a continuation fragment
- `skipToHttpData(msg)` — advances the read cursor past headers to the data section

---

### [Tools](psi/include/psi/tools/Tools.h)

Header-only utility functions in namespace `psi::tools`.

**String / encoding:**

| Function | Description |
|---|---|
| `to_hex_string(buffer, sz)` | Raw bytes → lowercase hex string |
| `to_hex_string(uint64_t)` | 64-bit integer → hex string (no leading zeros) |
| `ptr_to_address(ptr)` | Pointer → `"0x..."` hex string |
| `parse_to_map(s, delim)` | Newline-separated `key<delim>value` text → `vector<pair<string,string>>` |
| `parse_to_vector(s, delim)` | Delimited text → `vector<string>` |
| `ltrim(str, chars)` | Strip leading characters (default: whitespace) |
| `rtrim(str, chars)` | Strip trailing characters |
| `trim(str, chars)` | Strip both ends |
| `to_upper(str)` | ASCII uppercase copy |
| `utf8_to_wstring(str)` | UTF-8 `string` → `wstring` (cross-platform) |
| `wstring_to_utf8(wstr)` | `wstring` → UTF-8 `string` (cross-platform) |

**Time:**

| Function | Description |
|---|---|
| `to_iso_8601(tp)` | `time_point` → `"2026-05-07T12:00:00.000Z"` |
| `from_iso_8601(s)` | ISO-8601 UTC string → `time_point` |
| `optional_iso_8601(tp)` | `to_iso_8601` or empty string for epoch |
| `generateTimeStamp()` | Local time as `"YYYY.MM.DD_HH.MM.SS"` |

**Async:**

| Function | Description |
|---|---|
| `convertToSyncCall(fn, arg, timeout)` | Wraps a callback-based async function into a blocking call (default timeout: 10 s) |

```cpp
#include "psi/tools/Tools.h"
using namespace psi::tools;

// UTF-8 ↔ wstring
const auto s  = wstring_to_utf8(L"Привет мир");
const auto ws = utf8_to_wstring(s);

// ISO-8601 round-trip
const auto now = std::chrono::system_clock::now();
const auto iso = to_iso_8601(now);           // "2026-05-07T12:00:00.000Z"
const auto tp  = from_iso_8601(iso.c_str());

// Hex formatting
uint8_t data[] = {0xde, 0xad, 0xbe, 0xef};
std::cout << to_hex_string(data, 4);         // "deadbeef"
```

---

## Building

The project uses [CMake presets](CMakePresets.json). The recommended preset is `release-asan` (clang-cl, Ninja, AddressSanitizer + UBSan).

```sh
# Configure
cmake --preset release-asan

# Build
cmake --build build/release-asan

# Run tests
ctest --preset release-asan
```

Available presets: `debug`, `debug-asan`, `release`, `release-asan`.

**Dependencies** are fetched automatically via CMake `FetchContent`:
- `psi-shared` — shared CMake helpers
- `psi-test` — Google Test wrapper
- `psi-comm`, `psi-thread` — optional runtime dependencies

---

## Usage examples

| Example | Source |
|---|---|
| Tools (UTF-8, timestamp) | [psi/examples/1_ToolsExamples.cpp](psi/examples/1_ToolsExamples.cpp) |
| ByteBuffer read/write | [psi/examples/2_ByteBufferExamples.cpp](psi/examples/2_ByteBufferExamples.cpp) |
| Encryptor (AES-256) | [psi/examples/3_EncryptorExamples.cpp](psi/examples/3_EncryptorExamples.cpp) |
| BigInteger (Fibonacci) | [psi/examples/4_BigIntegerExamples.cpp](psi/examples/4_BigIntegerExamples.cpp) |

---

## License

See [LICENSE.md](LICENSE.md).
