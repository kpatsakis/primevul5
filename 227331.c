assembleVaryKey(String &vary, SBuf &vstr, const HttpRequest &request)
{
    static const SBuf asterisk("*");
    const char *pos = nullptr;
    const char *item = nullptr;
    int ilen = 0;

    while (strListGetItem(&vary, ',', &item, &ilen, &pos)) {
        SBuf name(item, ilen);
        if (name == asterisk) {
            vstr = asterisk;
            break;
        }
        name.toLower();
        if (!vstr.isEmpty())
            vstr.append(", ", 2);
        vstr.append(name);
        String hdr(request.header.getByName(name));
        const char *value = hdr.termedBuf();
        if (value) {
            value = rfc1738_escape_part(value);
            vstr.append("=\"", 2);
            vstr.append(value);
            vstr.append("\"", 1);
        }

        hdr.clean();
    }
}