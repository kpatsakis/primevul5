static avifBool avifParseItemInfoEntry(avifMeta * meta, const uint8_t * raw, size_t rawLen)
{
    BEGIN_STREAM(s, raw, rawLen);

    CHECK(avifROStreamReadAndEnforceVersion(&s, 2)); // TODO: support version > 2? 2+ is required for item_type

    uint16_t itemID;                                      // unsigned int(16) item_ID;
    CHECK(avifROStreamReadU16(&s, &itemID));              //
    uint16_t itemProtectionIndex;                         // unsigned int(16) item_protection_index;
    CHECK(avifROStreamReadU16(&s, &itemProtectionIndex)); //
    uint8_t itemType[4];                                  // unsigned int(32) item_type;
    CHECK(avifROStreamRead(&s, itemType, 4));             //

    avifContentType contentType;
    if (!memcmp(itemType, "mime", 4)) {
        CHECK(avifROStreamReadString(&s, NULL, 0));                                   // string item_name; (skipped)
        CHECK(avifROStreamReadString(&s, contentType.contentType, CONTENTTYPE_SIZE)); // string content_type;
    } else {
        memset(&contentType, 0, sizeof(contentType));
    }

    avifDecoderItem * item = avifMetaFindItem(meta, itemID);
    if (!item) {
        return AVIF_FALSE;
    }

    memcpy(item->type, itemType, sizeof(itemType));
    memcpy(&item->contentType, &contentType, sizeof(contentType));
    return AVIF_TRUE;
}