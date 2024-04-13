Header headerLink(Header h)
{
    if (h != NULL)
	h->nrefs++;
    return h;
}