avifBool avifDecoderIsKeyframe(const avifDecoder * decoder, uint32_t frameIndex)
{
    if ((decoder->data->tiles.count > 0) && decoder->data->tiles.tile[0].input) {
        if (frameIndex < decoder->data->tiles.tile[0].input->samples.count) {
            return decoder->data->tiles.tile[0].input->samples.sample[frameIndex].sync;
        }
    }
    return AVIF_FALSE;
}