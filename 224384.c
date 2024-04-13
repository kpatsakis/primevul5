static avifResult avifDecoderFlush(avifDecoder * decoder)
{
    avifDecoderDataResetCodec(decoder->data);

    for (unsigned int i = 0; i < decoder->data->tiles.count; ++i) {
        avifTile * tile = &decoder->data->tiles.tile[i];
        tile->codec = avifCodecCreateInternal(decoder->codecChoice, tile->input);
        if (!tile->codec) {
            return AVIF_RESULT_NO_CODEC_AVAILABLE;
        }
        if (!tile->codec->open(tile->codec, decoder->imageIndex + 1)) {
            return AVIF_RESULT_DECODE_COLOR_FAILED;
        }
    }
    return AVIF_RESULT_OK;
}