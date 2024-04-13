HttpStateData::~HttpStateData()
{
    /*
     * don't forget that ~Client() gets called automatically
     */

    if (httpChunkDecoder)
        delete httpChunkDecoder;

    cbdataReferenceDone(_peer);

    debugs(11,5, HERE << "HttpStateData " << this << " destroyed; " << serverConnection);
}