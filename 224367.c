static avifCodec * avifCodecCreateInternal(avifCodecChoice choice, avifCodecDecodeInput * decodeInput)
{
    avifCodec * codec = avifCodecCreate(choice, AVIF_CODEC_FLAG_CAN_DECODE);
    if (codec) {
        codec->decodeInput = decodeInput;
    }
    return codec;
}