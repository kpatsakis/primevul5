void avifCodecDecodeInputDestroy(avifCodecDecodeInput * decodeInput)
{
    avifArrayDestroy(&decodeInput->samples);
    avifFree(decodeInput);
}