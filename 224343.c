static avifMeta * avifMetaCreate()
{
    avifMeta * meta = (avifMeta *)avifAlloc(sizeof(avifMeta));
    memset(meta, 0, sizeof(avifMeta));
    avifArrayCreate(&meta->items, sizeof(avifDecoderItem), 8);
    avifArrayCreate(&meta->properties, sizeof(avifProperty), 16);
    avifArrayCreate(&meta->idats, sizeof(avifDecoderItemData), 1);
    return meta;
}