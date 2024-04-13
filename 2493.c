gopherStateFree(const CommCloseCbParams &params)
{
    GopherStateData *gopherState = (GopherStateData *)params.data;
    // Assume that FwdState is monitoring and calls noteClosure(). See XXX about
    // Connection sharing with FwdState in gopherStart().
    delete gopherState;
}