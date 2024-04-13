avifResult avifDecoderNextImage(avifDecoder * decoder)
{
    for (unsigned int tileIndex = 0; tileIndex < decoder->data->tiles.count; ++tileIndex) {
        avifTile * tile = &decoder->data->tiles.tile[tileIndex];

        if (!tile->codec->getNextImage(tile->codec, tile->image)) {
            if (tile->input->alpha) {
                return AVIF_RESULT_DECODE_ALPHA_FAILED;
            } else {
                if (tile->image->width) {
                    // We've sent at least one image, but we've run out now.
                    return AVIF_RESULT_NO_IMAGES_REMAINING;
                }
                return AVIF_RESULT_DECODE_COLOR_FAILED;
            }
        }
    }

    if (decoder->data->tiles.count != (decoder->data->colorTileCount + decoder->data->alphaTileCount)) {
        // TODO: assert here? This should be impossible.
        return AVIF_RESULT_UNKNOWN_ERROR;
    }

    if ((decoder->data->colorGrid.rows > 0) || (decoder->data->colorGrid.columns > 0)) {
        if (!avifDecoderDataFillImageGrid(
                decoder->data, &decoder->data->colorGrid, decoder->image, 0, decoder->data->colorTileCount, AVIF_FALSE)) {
            return AVIF_RESULT_INVALID_IMAGE_GRID;
        }
    } else {
        // Normal (most common) non-grid path. Just steal the planes from the only "tile".

        if (decoder->data->colorTileCount != 1) {
            return AVIF_RESULT_DECODE_COLOR_FAILED;
        }

        avifImage * srcColor = decoder->data->tiles.tile[0].image;

        if ((decoder->image->width != srcColor->width) || (decoder->image->height != srcColor->height) ||
            (decoder->image->depth != srcColor->depth)) {
            avifImageFreePlanes(decoder->image, AVIF_PLANES_ALL);

            decoder->image->width = srcColor->width;
            decoder->image->height = srcColor->height;
            decoder->image->depth = srcColor->depth;
        }

#if 0
        // This code is currently unnecessary as the CICP is always set by the end of avifDecoderParse().
        if (!decoder->data->cicpSet) {
            decoder->data->cicpSet = AVIF_TRUE;
            decoder->image->colorPrimaries = srcColor->colorPrimaries;
            decoder->image->transferCharacteristics = srcColor->transferCharacteristics;
            decoder->image->matrixCoefficients = srcColor->matrixCoefficients;
        }
#endif

        avifImageStealPlanes(decoder->image, srcColor, AVIF_PLANES_YUV);
    }

    if ((decoder->data->alphaGrid.rows > 0) || (decoder->data->alphaGrid.columns > 0)) {
        if (!avifDecoderDataFillImageGrid(
                decoder->data, &decoder->data->alphaGrid, decoder->image, decoder->data->colorTileCount, decoder->data->alphaTileCount, AVIF_TRUE)) {
            return AVIF_RESULT_INVALID_IMAGE_GRID;
        }
    } else {
        // Normal (most common) non-grid path. Just steal the planes from the only "tile".

        if (decoder->data->alphaTileCount == 0) {
            avifImageFreePlanes(decoder->image, AVIF_PLANES_A); // no alpha
        } else {
            if (decoder->data->alphaTileCount != 1) {
                return AVIF_RESULT_DECODE_ALPHA_FAILED;
            }

            avifImage * srcAlpha = decoder->data->tiles.tile[decoder->data->colorTileCount].image;
            if ((decoder->image->width != srcAlpha->width) || (decoder->image->height != srcAlpha->height) ||
                (decoder->image->depth != srcAlpha->depth)) {
                return AVIF_RESULT_DECODE_ALPHA_FAILED;
            }

            avifImageStealPlanes(decoder->image, srcAlpha, AVIF_PLANES_A);
        }
    }

    ++decoder->imageIndex;
    if (decoder->data->sourceSampleTable) {
        // Decoding from a track! Provide timing information.

        avifResult timingResult = avifDecoderNthImageTiming(decoder, decoder->imageIndex, &decoder->imageTiming);
        if (timingResult != AVIF_RESULT_OK) {
            return timingResult;
        }
    }
    return AVIF_RESULT_OK;
}