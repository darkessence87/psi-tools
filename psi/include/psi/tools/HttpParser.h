#pragma once

#include <map>

#include "psi/tools/ByteBuffer.h"

namespace psi::tools {

class HttpParser
{
public:
    /**
     * @brief Max message length to be parsed
     * 
     */
    static const size_t MAX_MSG_LENGTH;

    /**
     * @brief Parse message's header, meta and data block(s)
     * 
     * @param msg (in) message to be parsed
     * @param header (out) header to be filled in
     * @param meta (out) meta data to be filled in
     * @param remainingDataSz (in, out) remaining data length.
     * Usually equal to MAX_MSG_LENGTH as (in) parameter.
     * Return expected length of data block in next message as (out) parameter
     * @param data (in, out) stream object to be written in with data block(s)
     * @return 'true' if end of data is found (last data block length = 0)
     * @return 'false' if end of data is NOT found
     */
    static bool parseHttpChunkedMessage(const ByteBuffer &msg,
                                        std::string &header,
                                        std::map<std::string, std::string> &meta,
                                        size_t &remainingDataSz,
                                        std::ostringstream &data);

    /**
     * @brief Parse fragmented message's data block(s)
     * 
     * @param fragment (in) fragment to be parsed
     * @param remainingSz (in, out) remaining data length.
     * Equal to remaining length of first data block in fragment as (in) parameter.
     * Return expected length of data block in next fragment as (out) parameter
     * @param data (in, out) stream object to be written in with data block(s)
     * @return 'true' if end of data is found (last data block length = 0)
     * @return 'false' if end of data is NOT found
     */
    static bool parseHttpFragment(const ByteBuffer &fragment, size_t &remainingSz, std::ostringstream &data);

    /**
     * @brief Skip message to data block.
     * 
     * @param msg (in) message buffer
     */
    static void skipToHttpData(const ByteBuffer &msg);
};

} // namespace psi::tools