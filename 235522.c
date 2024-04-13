UTF16BEDecoder& utf16be_decoder()
{
    static UTF16BEDecoder* decoder;
    if (!decoder)
        decoder = new UTF16BEDecoder;
    return *decoder;
}