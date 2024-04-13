httpMakeVaryMark(HttpRequest * request, HttpReply const * reply)
{
    SBuf vstr;
    String vary;

    vary = reply->header.getList(Http::HdrType::VARY);
    assembleVaryKey(vary, vstr, *request);

#if X_ACCELERATOR_VARY
    vary.clean();
    vary = reply->header.getList(Http::HdrType::HDR_X_ACCELERATOR_VARY);
    assembleVaryKey(vary, vstr, *request);
#endif

    debugs(11, 3, vstr);
    return vstr;
}