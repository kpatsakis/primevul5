static avifDecoderData * avifDecoderDataCreate()
{
    avifDecoderData * data = (avifDecoderData *)avifAlloc(sizeof(avifDecoderData));
    memset(data, 0, sizeof(avifDecoderData));
    data->meta = avifMetaCreate();
    avifArrayCreate(&data->tracks, sizeof(avifTrack), 2);
    avifArrayCreate(&data->tiles, sizeof(avifTile), 8);
    return data;
}