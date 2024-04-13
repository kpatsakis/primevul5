http_VSLH(const struct http *hp, unsigned hdr)
{
        int i;

        if (hp->vsl != NULL) {
                AN(hp->vsl->wid & (VSL_CLIENTMARKER|VSL_BACKENDMARKER));
                i = hdr;
                if (i > HTTP_HDR_FIRST)
                        i = HTTP_HDR_FIRST;
                i += hp->logtag;
                VSLbt(hp->vsl, (enum VSL_tag_e)i, hp->hd[hdr]);
        }
}