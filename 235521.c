Latin2Decoder& latin2_decoder()
{
    static Latin2Decoder* decoder = nullptr;
    if (!decoder)
        decoder = new Latin2Decoder;
    return *decoder;
}