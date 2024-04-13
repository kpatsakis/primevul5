rfbSetClientColourMap(rfbClientPtr cl, int firstColour, int nColours)
{
    if (cl->screen->serverFormat.trueColour || !cl->readyForSetColourMapEntries) {
	return TRUE;
    }

    if (nColours == 0) {
	nColours = cl->screen->colourMap.count;
    }

    if (cl->format.trueColour) {
	LOCK(cl->updateMutex);
	(*rfbInitColourMapSingleTableFns
	    [BPP2OFFSET(cl->format.bitsPerPixel)]) (&cl->translateLookupTable,
					     &cl->screen->serverFormat, &cl->format,&cl->screen->colourMap);

	sraRgnDestroy(cl->modifiedRegion);
	cl->modifiedRegion =
	  sraRgnCreateRect(0,0,cl->screen->width,cl->screen->height);
	UNLOCK(cl->updateMutex);

	return TRUE;
    }

    return rfbSendSetColourMapEntries(cl, firstColour, nColours);
}