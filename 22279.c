static int normalise_attribs(struct parse_state *state, int attribs)
{
    int nattribs = 0;
    static int deprecated_warnings = 0;

    /* always provide size.shared if value.shared specified */
    if ((attribs & ATTRIB_VALUE_SHARED))
        nattribs |= ATTRIB_VALUE_SHARED|ATTRIB_SIZE_SHARED;

    /* likewise size.priv */
    if ((attribs & ATTRIB_VALUE_PRIV))
        nattribs |= ATTRIB_VALUE_PRIV|ATTRIB_SIZE_PRIV;

    /* ignore any other specified attributes */

    if ((attribs & ATTRIB_DEPRECATED)) {
        if (!deprecated_warnings++)
            parse_error(state, "deprecated attribute names such as "
                                "content-type or modified-since (ignoring)");
    }

    return nattribs;
}