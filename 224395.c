avifResult avifDecoderRead(avifDecoder * decoder, avifImage * image, const avifROData * input)
{
    avifResult result = avifDecoderParse(decoder, input);
    if (result != AVIF_RESULT_OK) {
        return result;
    }
    result = avifDecoderNextImage(decoder);
    if (result != AVIF_RESULT_OK) {
        return result;
    }
    avifImageCopy(image, decoder->image, AVIF_PLANES_ALL);
    return AVIF_RESULT_OK;
}