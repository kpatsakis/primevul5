PrintPixelFormat(rfbPixelFormat *pf)
{
    if (pf->bitsPerPixel == 1) {
        rfbLog("  1 bpp, %s sig bit in each byte is leftmost on the screen.\n",
               (pf->bigEndian ? "most" : "least"));
    } else {
        rfbLog("  %d bpp, depth %d%s\n",pf->bitsPerPixel,pf->depth,
               ((pf->bitsPerPixel == 8) ? ""
                : (pf->bigEndian ? ", big endian" : ", little endian")));
        if (pf->trueColour) {
            rfbLog("  true colour: max r %d g %d b %d, shift r %d g %d b %d\n",
                   pf->redMax, pf->greenMax, pf->blueMax,
                   pf->redShift, pf->greenShift, pf->blueShift);
        } else {
            rfbLog("  uses a colour map (not true colour).\n");
        }
    }
}