Latin1Decoder& latin1_decoder()
{
    static Latin1Decoder* decoder;
    if (!decoder)
        decoder = new Latin1Decoder;
    return *decoder;
}