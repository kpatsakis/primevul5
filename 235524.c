Decoder* decoder_for(const String& a_encoding)
{
    auto encoding = get_standardized_encoding(a_encoding);
    if (encoding.equals_ignoring_case("windows-1252"))
        return &latin1_decoder();
    if (encoding.equals_ignoring_case("utf-8"))
        return &utf8_decoder();
    if (encoding.equals_ignoring_case("utf-16be"))
        return &utf16be_decoder();
    if (encoding.equals_ignoring_case("iso-8859-2"))
        return &latin2_decoder();
    dbgln("TextCodec: No decoder implemented for encoding '{}'", a_encoding);
    return nullptr;
}