static avifDecoderItem * avifMetaFindItem(avifMeta * meta, uint32_t itemID)
{
    if (itemID == 0) {
        return NULL;
    }

    for (uint32_t i = 0; i < meta->items.count; ++i) {
        if (meta->items.item[i].id == itemID) {
            return &meta->items.item[i];
        }
    }

    avifDecoderItem * item = (avifDecoderItem *)avifArrayPushPtr(&meta->items);
    avifArrayCreate(&item->properties, sizeof(avifProperty), 16);
    item->id = itemID;
    item->meta = meta;
    return item;
}