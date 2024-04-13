int headerGet(Header h, rpmTagVal tag, rpmtd td, headerGetFlags flags)
{
    int rc;
    headerTagTagFunction tagfunc = intGetTdEntry;

    if (td == NULL) return 0;

    rpmtdReset(td);
    td->tag = tag;

    if (flags & HEADERGET_EXT) {
	headerTagTagFunction extfunc = rpmHeaderTagFunc(tag);
	if (extfunc) tagfunc = extfunc;
    }
    rc = tagfunc(h, td, flags);

    assert(tag == td->tag);
    return rc;
}