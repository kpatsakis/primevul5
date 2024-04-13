DwaCompressor::setupChannelData (int minX, int minY, int maxX, int maxY)
{
    char *planarUncBuffer[NUM_COMPRESSOR_SCHEMES];

    for (int i=0; i<NUM_COMPRESSOR_SCHEMES; ++i)
    {
        planarUncBuffer[i] = 0;

        if (_planarUncBuffer[i])
            planarUncBuffer[i] =  _planarUncBuffer[i];
    }

    for (unsigned int chan = 0; chan < _channelData.size(); ++chan)
    {
        ChannelData *cd = &_channelData[chan];

        cd->width  = OPENEXR_IMF_NAMESPACE::numSamples (cd->xSampling, minX, maxX);
        cd->height = OPENEXR_IMF_NAMESPACE::numSamples (cd->ySampling, minY, maxY);
                                
        cd->planarUncSize =
            cd->width * cd->height * OPENEXR_IMF_NAMESPACE::pixelTypeSize (cd->type);
                                  
        cd->planarUncBuffer    = planarUncBuffer[cd->compression];
        cd->planarUncBufferEnd = cd->planarUncBuffer;

        cd->planarUncRle[0]    = cd->planarUncBuffer;
        cd->planarUncRleEnd[0] = cd->planarUncRle[0];

        for (int byte = 1; byte < OPENEXR_IMF_NAMESPACE::pixelTypeSize(cd->type); ++byte)
        {
            cd->planarUncRle[byte] = 
                         cd->planarUncRle[byte-1] + cd->width * cd->height;

            cd->planarUncRleEnd[byte] =
                         cd->planarUncRle[byte];
        }

        cd->planarUncType = cd->type;

        if (cd->compression == LOSSY_DCT)
        {
            cd->planarUncType = FLOAT;
        }
        else
        {
            planarUncBuffer[cd->compression] +=
                cd->width * cd->height * OPENEXR_IMF_NAMESPACE::pixelTypeSize (cd->planarUncType);
        }
    }
}