rfbSetClientColourMaps(rfbScreenInfoPtr rfbScreen, int firstColour, int nColours)
{
    rfbClientIteratorPtr i;
    rfbClientPtr cl;

    i = rfbGetClientIterator(rfbScreen);
    while((cl = rfbClientIteratorNext(i)))
      rfbSetClientColourMap(cl, firstColour, nColours);
    rfbReleaseClientIterator(i);
}