clientPackRangeHdr(const HttpReply * rep, const HttpHdrRangeSpec * spec, String boundary, MemBuf * mb)
{
    HttpHeader hdr(hoReply);
    assert(rep);
    assert(spec);

    /* put boundary */
    debugs(33, 5, "appending boundary: " << boundary);
    /* rfc2046 requires to _prepend_ boundary with <crlf>! */
    mb->appendf("\r\n--" SQUIDSTRINGPH "\r\n", SQUIDSTRINGPRINT(boundary));

    /* stuff the header with required entries and pack it */

    if (rep->header.has(Http::HdrType::CONTENT_TYPE))
        hdr.putStr(Http::HdrType::CONTENT_TYPE, rep->header.getStr(Http::HdrType::CONTENT_TYPE));

    httpHeaderAddContRange(&hdr, *spec, rep->content_length);

    hdr.packInto(mb);
    hdr.clean();

    /* append <crlf> (we packed a header, not a reply) */
    mb->append("\r\n", 2);
}