parse_attr(struct ly_ctx *ctx, const char *data, unsigned int *len, struct lyxml_elem *parent)
{
    const char *c = data, *start, *delim;
    char *prefix = NULL, xml_flag, *str;
    int uc;
    struct lyxml_attr *attr = NULL, *a;
    unsigned int size;

    /* check if it is attribute or namespace */
    if (!strncmp(c, "xmlns", 5)) {
        /* namespace */
        attr = calloc(1, sizeof (struct lyxml_ns));
        LY_CHECK_ERR_RETURN(!attr, LOGMEM(ctx), NULL);

        attr->type = LYXML_ATTR_NS;
        ((struct lyxml_ns *)attr)->parent = parent;
        c += 5;
        if (*c != ':') {
            /* default namespace, prefix will be empty */
            goto equal;
        }
        c++;                    /* go after ':' to the prefix value */
    } else {
        /* attribute */
        attr = calloc(1, sizeof *attr);
        LY_CHECK_ERR_RETURN(!attr, LOGMEM(ctx), NULL);

        attr->type = LYXML_ATTR_STD;
    }

    /* process name part of the attribute */
    start = c;
    uc = lyxml_getutf8(ctx, c, &size);
    if (!is_xmlnamestartchar(uc)) {
        LOGVAL(ctx, LYE_XML_INVAL, LY_VLOG_NONE, NULL, "NameStartChar of the attribute");
        free(attr);
        return NULL;
    }
    xml_flag = 4;
    if (*c == 'x') {
        xml_flag = 1;
    }
    c += size;
    uc = lyxml_getutf8(ctx, c, &size);
    while (is_xmlnamechar(uc)) {
        if (attr->type == LYXML_ATTR_STD) {
            if ((*c == ':') && (xml_flag != 3)) {
                /* attribute in a namespace (but disregard the special "xml" namespace) */
                start = c + 1;

                /* look for the prefix in namespaces */
                if (prefix) {
                    LOGVAL(ctx, LYE_XML_INVAL, LY_VLOG_NONE, NULL, "prefix start, \":\" already parsed");
                    goto error;
                }
                prefix = malloc((c - data + 1) * sizeof *prefix);
                LY_CHECK_ERR_GOTO(!prefix, LOGMEM(ctx), error);
                memcpy(prefix, data, c - data);
                prefix[c - data] = '\0';
                attr->ns = lyxml_get_ns(parent, prefix);
                if (!attr->ns) {
                    /* remember the prefix for later resolution */
                    attr->type = LYXML_ATTR_STD_UNRES;
                    attr->ns = (struct lyxml_ns *)prefix;
                    prefix = NULL;
                }
            } else if (((*c == 'm') && (xml_flag == 1)) ||
                    ((*c == 'l') && (xml_flag == 2))) {
                ++xml_flag;
            } else {
                xml_flag = 4;
            }
        }
        c += size;
        uc = lyxml_getutf8(ctx, c, &size);
    }

    /* store the name */
    size = c - start;
    attr->name = lydict_insert(ctx, start, size);

equal:
    /* check Eq mark that can be surrounded by whitespaces */
    ign_xmlws(c);
    if (*c != '=') {
        LOGVAL(ctx, LYE_XML_INVAL, LY_VLOG_NONE, NULL, "attribute definition, \"=\" expected");
        goto error;
    }
    c++;
    ign_xmlws(c);

    /* process value part of the attribute */
    if (!*c || (*c != '"' && *c != '\'')) {
        LOGVAL(ctx, LYE_XML_INVAL, LY_VLOG_NONE, NULL, "attribute value, \" or \' expected");
        goto error;
    }
    delim = c;
    str = parse_text(ctx, ++c, *delim, &size);
    if (!str && !size) {
        goto error;
    }
    attr->value = lydict_insert_zc(ctx, str);

    *len = c + size + 1 - data; /* +1 is delimiter size */

    /* put attribute into the parent's attributes list */
    if (parent->attr) {
        /* go to the end of the list */
        for (a = parent->attr; a->next; a = a->next);
        /* and append new attribute */
        a->next = attr;
    } else {
        /* add the first attribute in the list */
        parent->attr = attr;
    }

    free(prefix);
    return attr;

error:
    lyxml_free_attr(ctx, NULL, attr);
    free(prefix);
    return NULL;
}