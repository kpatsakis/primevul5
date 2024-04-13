gopherCachable(const HttpRequest * req)
{
    int cachable = 1;
    char type_id;
    /* parse to see type */
    gopher_request_parse(req,
                         &type_id,
                         NULL);

    switch (type_id) {

    case GOPHER_INDEX:

    case GOPHER_CSO:

    case GOPHER_TELNET:

    case GOPHER_3270:
        cachable = 0;
        break;

    default:
        cachable = 1;
    }

    return cachable;
}