static avifBool avifDecoderDataGenerateImageGridTiles(avifDecoderData * data, avifImageGrid * grid, avifDecoderItem * gridItem, avifBool alpha)
{
    unsigned int tilesRequested = (unsigned int)grid->rows * (unsigned int)grid->columns;

    // Count number of dimg for this item, bail out if it doesn't match perfectly
    unsigned int tilesAvailable = 0;
    for (uint32_t i = 0; i < gridItem->meta->items.count; ++i) {
        avifDecoderItem * item = &gridItem->meta->items.item[i];
        if (item->dimgForID == gridItem->id) {
            if (memcmp(item->type, "av01", 4)) {
                continue;
            }
            if (item->hasUnsupportedEssentialProperty) {
                // An essential property isn't supported by libavif; ignore the item.
                continue;
            }

            ++tilesAvailable;
        }
    }

    if (tilesRequested != tilesAvailable) {
        return AVIF_FALSE;
    }

    avifBool firstTile = AVIF_TRUE;
    for (uint32_t i = 0; i < gridItem->meta->items.count; ++i) {
        avifDecoderItem * item = &gridItem->meta->items.item[i];
        if (item->dimgForID == gridItem->id) {
            if (memcmp(item->type, "av01", 4)) {
                continue;
            }
            if (item->hasUnsupportedEssentialProperty) {
                // An essential property isn't supported by libavif; ignore the item.
                continue;
            }

            avifTile * tile = avifDecoderDataCreateTile(data);
            avifDecodeSample * sample = (avifDecodeSample *)avifArrayPushPtr(&tile->input->samples);
            sample->data.data = avifDecoderDataCalcItemPtr(data, item);
            sample->data.size = item->size;
            sample->sync = AVIF_TRUE;
            tile->input->alpha = alpha;

            if (firstTile) {
                firstTile = AVIF_FALSE;

                // Adopt the av1C property of the first av01 tile, so that it can be queried from
                // the top-level color/alpha item during avifDecoderReset().
                const avifProperty * srcProp = avifPropertyArrayFind(&item->properties, "av1C");
                if (!srcProp) {
                    return AVIF_FALSE;
                }
                avifProperty * dstProp = (avifProperty *)avifArrayPushPtr(&gridItem->properties);
                memcpy(dstProp, srcProp, sizeof(avifProperty));
            }
        }
    }
    return AVIF_TRUE;
}