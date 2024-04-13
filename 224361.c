static avifTrack * avifDecoderDataCreateTrack(avifDecoderData * data)
{
    avifTrack * track = (avifTrack *)avifArrayPushPtr(&data->tracks);
    track->meta = avifMetaCreate();
    return track;
}