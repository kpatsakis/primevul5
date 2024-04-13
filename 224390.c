static void avifMetaDestroy(avifMeta * meta)
{
    for (uint32_t i = 0; i < meta->items.count; ++i) {
        avifDecoderItem * item = &meta->items.item[i];
        avifArrayDestroy(&item->properties);
    }
    avifArrayDestroy(&meta->items);
    avifArrayDestroy(&meta->properties);
    avifArrayDestroy(&meta->idats);
    avifFree(meta);
}