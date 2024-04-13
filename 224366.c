static avifBool avifDecoderDataFindMetadata(avifDecoderData * data, avifMeta * meta, avifImage * image, uint32_t colorId)
{
    avifROData exifData = AVIF_DATA_EMPTY;
    avifROData xmpData = AVIF_DATA_EMPTY;

    for (uint32_t itemIndex = 0; itemIndex < meta->items.count; ++itemIndex) {
        avifDecoderItem * item = &meta->items.item[itemIndex];
        if (!item->size) {
            continue;
        }
        if (item->hasUnsupportedEssentialProperty) {
            // An essential property isn't supported by libavif; ignore the item.
            continue;
        }

        if ((colorId > 0) && (item->descForID != colorId)) {
            // Not a content description (metadata) for the colorOBU, skip it
            continue;
        }

        if (!memcmp(item->type, "Exif", 4)) {
            // Advance past Annex A.2.1's header
            const uint8_t * boxPtr = avifDecoderDataCalcItemPtr(data, item);
            BEGIN_STREAM(exifBoxStream, boxPtr, item->size);
            uint32_t exifTiffHeaderOffset;
            CHECK(avifROStreamReadU32(&exifBoxStream, &exifTiffHeaderOffset)); // unsigned int(32) exif_tiff_header_offset;

            exifData.data = avifROStreamCurrent(&exifBoxStream);
            exifData.size = avifROStreamRemainingBytes(&exifBoxStream);
        } else if (!memcmp(item->type, "mime", 4) && !memcmp(item->contentType.contentType, xmpContentType, xmpContentTypeSize)) {
            xmpData.data = avifDecoderDataCalcItemPtr(data, item);
            xmpData.size = item->size;
        }
    }

    if (exifData.data && exifData.size) {
        avifImageSetMetadataExif(image, exifData.data, exifData.size);
    }
    if (xmpData.data && xmpData.size) {
        avifImageSetMetadataXMP(image, xmpData.data, xmpData.size);
    }
    return AVIF_TRUE;
}