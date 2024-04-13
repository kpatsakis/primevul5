HttpStateData::maybeMakeSpaceAvailable(bool doGrow)
{
    // how much we are allowed to buffer
    const int limitBuffer = (flags.headers_parsed ? Config.readAheadGap : Config.maxReplyHeaderSize);

    if (limitBuffer < 0 || inBuf.length() >= (SBuf::size_type)limitBuffer) {
        // when buffer is at or over limit already
        debugs(11, 7, "will not read up to " << limitBuffer << ". buffer has (" << inBuf.length() << "/" << inBuf.spaceSize() << ") from " << serverConnection);
        debugs(11, DBG_DATA, "buffer has {" << inBuf << "}");
        // Process next response from buffer
        processReply();
        return false;
    }

    // how much we want to read
    const size_t read_size = calcBufferSpaceToReserve(inBuf.spaceSize(), (limitBuffer - inBuf.length()));

    if (!read_size) {
        debugs(11, 7, "will not read up to " << read_size << " into buffer (" << inBuf.length() << "/" << inBuf.spaceSize() << ") from " << serverConnection);
        return false;
    }

    // just report whether we could grow or not, do not actually do it
    if (doGrow)
        return (read_size >= 2);

    // we may need to grow the buffer
    inBuf.reserveSpace(read_size);
    debugs(11, 8, (!flags.do_next_read ? "will not" : "may") <<
           " read up to " << read_size << " bytes info buf(" << inBuf.length() << "/" << inBuf.spaceSize() <<
           ") from " << serverConnection);

    return (inBuf.spaceSize() >= 2); // only read if there is 1+ bytes of space available
}