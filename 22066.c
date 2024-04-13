status_add_symbol_id(ushort * idlist, int nid, ushort new_id)
{
    int i;
    ushort *idp;
    ushort t1, t2;

    for (i = 0, idp = idlist; i < nid; i++)
        if (new_id <= *idp)
            break;
    if (new_id == *idp)         /* duplicate item */
        return nid;
    /* insert new_id in front of *idp */
    for (t1 = new_id; i < nid; i++) {
        t2 = *idp;
        *idp++ = t1;
        t1 = t2;
    }
    *idp = t1;
    return nid + 1;
}