status_print_idlist(stream * s, const ushort * idlist, int nid,
                    const char *title)
{
    int i;

    for (i = 0; i < nid; i++) {
        char idstr[6];          /* ddddL and a null */
        int n, l;

        n = idlist[i] >> 6;
        l = (idlist[i] & 077) + 'A' - 1;
        gs_snprintf(idstr, sizeof(idstr), "%d%c", n, l);
        status_put_id(s, title, idstr);
    }
    status_end_id_list(s);
}