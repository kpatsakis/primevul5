HttpStateData::httpStateConnClosed(const CommCloseCbParams &params)
{
    debugs(11, 5, "httpStateFree: FD " << params.fd << ", httpState=" << params.data);
    doneWithFwd = "httpStateConnClosed()"; // assume FwdState is monitoring too
    mustStop("HttpStateData::httpStateConnClosed");
}