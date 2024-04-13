HttpStateData::decodeAndWriteReplyBody()
{
    const char *data = NULL;
    int len;
    bool wasThereAnException = false;
    assert(flags.chunked);
    assert(httpChunkDecoder);
    SQUID_ENTER_THROWING_CODE();
    MemBuf decodedData;
    decodedData.init();
    httpChunkDecoder->setPayloadBuffer(&decodedData);
    const bool doneParsing = httpChunkDecoder->parse(inBuf);
    inBuf = httpChunkDecoder->remaining(); // sync buffers after parse
    len = decodedData.contentSize();
    data=decodedData.content();
    addVirginReplyBody(data, len);
    if (doneParsing) {
        lastChunk = 1;
        flags.do_next_read = false;
    }
    SQUID_EXIT_THROWING_CODE(wasThereAnException);
    return wasThereAnException;
}