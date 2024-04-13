avifResult avifDecoderParse(avifDecoder * decoder, const avifROData * rawInput)
{
    // Cleanup anything lingering in the decoder
    avifDecoderCleanup(decoder);

    // -----------------------------------------------------------------------
    // Parse BMFF boxes

    decoder->data = avifDecoderDataCreate();

    // Shallow copy, on purpose
    memcpy(&decoder->data->rawInput, rawInput, sizeof(avifROData));

    if (!avifParse(decoder->data, decoder->data->rawInput.data, decoder->data->rawInput.size)) {
        return AVIF_RESULT_BMFF_PARSE_FAILED;
    }

    avifBool avifCompatible = avifFileTypeIsCompatible(&decoder->data->ftyp);
    if (!avifCompatible) {
        return AVIF_RESULT_INVALID_FTYP;
    }

    // Sanity check items
    for (uint32_t itemIndex = 0; itemIndex < decoder->data->meta->items.count; ++itemIndex) {
        avifDecoderItem * item = &decoder->data->meta->items.item[itemIndex];
        if (item->hasUnsupportedEssentialProperty) {
            // An essential property isn't supported by libavif; ignore the item.
            continue;
        }
        const uint8_t * p = avifDecoderDataCalcItemPtr(decoder->data, item);
        if (p == NULL) {
            return AVIF_RESULT_BMFF_PARSE_FAILED;
        }
    }

    // Sanity check tracks
    for (uint32_t trackIndex = 0; trackIndex < decoder->data->tracks.count; ++trackIndex) {
        avifTrack * track = &decoder->data->tracks.track[trackIndex];
        if (!track->sampleTable) {
            continue;
        }

        for (uint32_t chunkIndex = 0; chunkIndex < track->sampleTable->chunks.count; ++chunkIndex) {
            avifSampleTableChunk * chunk = &track->sampleTable->chunks.chunk[chunkIndex];
            if (chunk->offset > decoder->data->rawInput.size) {
                return AVIF_RESULT_BMFF_PARSE_FAILED;
            }
        }
    }
    return avifDecoderReset(decoder);
}