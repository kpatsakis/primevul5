void * headerUnload(Header h)
{
    return headerExport(h, NULL);
}