UTF8Decoder& utf8_decoder()
{
    static UTF8Decoder* decoder;
    if (!decoder)
        decoder = new UTF8Decoder;
    return *decoder;
}