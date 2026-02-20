#include "psi/tools/HttpParser.h"

namespace psi::tools {

const size_t HttpParser::MAX_MSG_LENGTH = static_cast<size_t>(-1);

bool HttpParser::parseHttpChunkedMessage(const ByteBuffer &msg,
                                         std::string &header,
                                         std::map<std::string, std::string> &meta,
                                         size_t &remainingDataSz,
                                         std::ostringstream &data)
{
    header.clear();
    meta.clear();

    remainingDataSz = MAX_MSG_LENGTH;
    if (msg.length() == 0) {
        return true;
    }

    msg.readLine(header);
    std::string line;
    uint8_t delims[1] = {'\n'};
    while (msg.readLine(line, delims, 1) || !line.empty()) {
        if (line.empty() || line == "\r") {
            break;
        }
        auto pos = line.find_first_of(':');
        if (pos != std::string::npos) {
            meta.emplace(to_upper(line.substr(0, pos)), line.substr(pos + 2, line.size() - pos - 3));
        }
    }

    return parseHttpFragment(msg, remainingDataSz, data);
}

bool HttpParser::parseHttpFragment(const ByteBuffer &fragment, size_t &remainingSz, std::ostringstream &data)
{
    bool endOfData = false;
    std::string line;
    while (fragment.readLine(line) || !line.empty()) {
        if (line.empty()) {
            continue;
        }
        bool isNotSizeLine = false;
        if (remainingSz == MAX_MSG_LENGTH) {
            for (auto c : line) {
                if (!std::isxdigit(c)) {
                    isNotSizeLine = true;
                    break;
                }
            }

            if (isNotSizeLine) {
                remainingSz -= line.size();
                data << line << "\r\n";
                continue;
            }

            remainingSz = line.empty() ? 0 : std::stoull(line, nullptr, 16);
            endOfData = remainingSz == 0;
            continue;
        }
        remainingSz -= line.size();
        if (!remainingSz) {
            remainingSz = MAX_MSG_LENGTH;
        }

        data << line;
    }

    return endOfData;
}

void HttpParser::skipToHttpData(const ByteBuffer &msg)
{
    uint32_t endLine = 0;
    uint8_t b = 0;
    while (msg.read(b)) {
        endLine = (endLine << 8) | b;
        if (endLine == 0x0d0a0d0a) {
            break;
        }
    }
}

} // namespace psi::tools
