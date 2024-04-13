avifResult avifDecoderNthImage(avifDecoder * decoder, uint32_t frameIndex)
{
    int requestedIndex = (int)frameIndex;
    if (requestedIndex == decoder->imageIndex) {
        // We're here already, nothing to do
        return AVIF_RESULT_OK;
    }

    if (requestedIndex == (decoder->imageIndex + 1)) {
        // it's just the next image, nothing special here
        return avifDecoderNextImage(decoder);
    }

    if (requestedIndex >= decoder->imageCount) {
        // Impossible index
        return AVIF_RESULT_NO_IMAGES_REMAINING;
    }

    // If we get here, a decoder flush is necessary
    decoder->imageIndex = ((int)avifDecoderNearestKeyframe(decoder, frameIndex)) - 1; // prepare to read nearest keyframe
    avifDecoderFlush(decoder);
    for (;;) {
        avifResult result = avifDecoderNextImage(decoder);
        if (result != AVIF_RESULT_OK) {
            return result;
        }

        if (requestedIndex == decoder->imageIndex) {
            break;
        }
    }
    return AVIF_RESULT_OK;
}