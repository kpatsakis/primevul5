http_VSLH_del(const struct http *hp, unsigned hdr)
{
        int i;

        if (hp->vsl != NULL) {
                /* We don't support unsetting stuff in the first line */
                assert (hdr >= HTTP_HDR_FIRST);
                AN(hp->vsl->wid & (VSL_CLIENTMARKER|VSL_BACKENDMARKER));
                i = (HTTP_HDR_UNSET - HTTP_HDR_METHOD);
                i += hp->logtag;
                VSLbt(hp->vsl, (enum VSL_tag_e)i, hp->hd[hdr]);
        }
}