HttpStateData::sentRequestBody(const CommIoCbParams &io)
{
    if (io.size > 0)
        statCounter.server.http.kbytes_out += io.size;

    Client::sentRequestBody(io);
}