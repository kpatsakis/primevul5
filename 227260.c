HttpStateData::truncateVirginBody()
{
    assert(flags.headers_parsed);

    HttpReply *vrep = virginReply();
    int64_t clen = -1;
    if (!vrep->expectingBody(request->method, clen) || clen < 0)
        return; // no body or a body of unknown size, including chunked

    if (payloadSeen - payloadTruncated <= clen)
        return; // we did not read too much or already took care of the extras

    if (const int64_t extras = payloadSeen - payloadTruncated - clen) {
        // server sent more that the advertised content length
        debugs(11, 5, "payloadSeen=" << payloadSeen <<
               " clen=" << clen << '/' << vrep->content_length <<
               " trucated=" << payloadTruncated << '+' << extras);

        inBuf.chop(0, inBuf.length() - extras);
        payloadTruncated += extras;
    }
}