static avifTile * avifDecoderDataCreateTile(avifDecoderData * data)
{
    avifTile * tile = (avifTile *)avifArrayPushPtr(&data->tiles);
    tile->image = avifImageCreateEmpty();
    tile->input = avifCodecDecodeInputCreate();
    return tile;
}