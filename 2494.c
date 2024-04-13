gopher_request_parse(const HttpRequest * req, char *type_id, char *request)
{
    ::Parser::Tokenizer tok(req->url.path());

    if (request)
        *request = 0;

    tok.skip('/'); // ignore failures? path could be ab-empty

    if (tok.atEnd()) {
        *type_id = GOPHER_DIRECTORY;
        return;
    }

    static const CharacterSet anyByte("UTF-8",0x00, 0xFF);

    SBuf typeId;
    (void)tok.prefix(typeId, anyByte, 1); // never fails since !atEnd()
    *type_id = typeId[0];

    if (request) {
        SBufToCstring(request, tok.remaining().substr(0, MAX_URL-1));
        /* convert %xx to char */
        rfc1738_unescape(request);
    }
}