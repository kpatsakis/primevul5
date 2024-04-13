gopherMimeCreate(GopherStateData * gopherState)
{
    StoreEntry *entry = gopherState->entry;
    const char *mime_type = NULL;
    const char *mime_enc = NULL;

    switch (gopherState->type_id) {

    case GOPHER_DIRECTORY:

    case GOPHER_INDEX:

    case GOPHER_HTML:

    case GOPHER_WWW:

    case GOPHER_CSO:
        mime_type = "text/html";
        break;

    case GOPHER_GIF:

    case GOPHER_IMAGE:

    case GOPHER_PLUS_IMAGE:
        mime_type = "image/gif";
        break;

    case GOPHER_SOUND:

    case GOPHER_PLUS_SOUND:
        mime_type = "audio/basic";
        break;

    case GOPHER_PLUS_MOVIE:
        mime_type = "video/mpeg";
        break;

    case GOPHER_MACBINHEX:

    case GOPHER_DOSBIN:

    case GOPHER_UUENCODED:

    case GOPHER_BIN:
        /* Rightnow We have no idea what it is. */
        mime_enc = mimeGetContentEncoding(gopherState->request);
        mime_type = mimeGetContentType(gopherState->request);
        if (!mime_type)
            mime_type = def_gopher_bin;
        break;

    case GOPHER_FILE:

    default:
        mime_enc = mimeGetContentEncoding(gopherState->request);
        mime_type = mimeGetContentType(gopherState->request);
        if (!mime_type)
            mime_type = def_gopher_text;
        break;
    }

    assert(entry->isEmpty());

    HttpReply *reply = new HttpReply;
    entry->buffer();
    reply->setHeaders(Http::scOkay, "Gatewaying", mime_type, -1, -1, -2);
    if (mime_enc)
        reply->header.putStr(Http::HdrType::CONTENT_ENCODING, mime_enc);

    entry->replaceHttpReply(reply);
    gopherState->reply_ = reply;
}