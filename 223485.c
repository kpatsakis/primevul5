dump_elem(struct lyout *out, const struct lyxml_elem *e, int level, int options, int last_elem)
{
    int size = 0;
    struct lyxml_attr *a;
    struct lyxml_elem *child;
    const char *delim, *delim_outer;
    int indent;

    if (!e->name) {
        /* mixed content */
        if (e->content) {
            return lyxml_dump_text(out, e->content, LYXML_DATA_ELEM);
        } else {
            return 0;
        }
    }

    delim = delim_outer = (options & LYXML_PRINT_FORMAT) ? "\n" : "";
    indent = 2 * level;
    if ((e->flags & LYXML_ELEM_MIXED) || (e->parent && (e->parent->flags & LYXML_ELEM_MIXED))) {
        delim = "";
    }
    if (e->parent && (e->parent->flags & LYXML_ELEM_MIXED)) {
        delim_outer = "";
        indent = 0;
    }
    if (last_elem && (options & LYXML_PRINT_NO_LAST_NEWLINE)) {
        delim_outer = "";
    }

    if (!(options & (LYXML_PRINT_OPEN | LYXML_PRINT_CLOSE | LYXML_PRINT_ATTRS)) || (options & LYXML_PRINT_OPEN))  {
        /* opening tag */
        if (e->ns && e->ns->prefix) {
            size += ly_print(out, "%*s<%s:%s", indent, "", e->ns->prefix, e->name);
        } else {
            size += ly_print(out, "%*s<%s", indent, "", e->name);
        }
    } else if (options & LYXML_PRINT_CLOSE) {
        indent = 0;
        goto close;
    }

    /* attributes */
    for (a = e->attr; a; a = a->next) {
        if (a->type == LYXML_ATTR_NS) {
            if (a->name) {
                size += ly_print(out, " xmlns:%s=\"", a->name);
            } else {
                size += ly_print(out, " xmlns=\"");
            }
        } else if (a->ns && a->ns->prefix) {
            size += ly_print(out, " %s:%s=\"", a->ns->prefix, a->name);
        } else {
            size += ly_print(out, " %s=\"", a->name);
        }

        if (a->value) {
            size += lyxml_dump_text(out, a->value, LYXML_DATA_ATTR);
        } else {
            size += ly_print(out, "&quot;&quot;");
        }
        size += ly_print(out, "\"");
    }

    /* apply options */
    if ((options & LYXML_PRINT_CLOSE) && (options & LYXML_PRINT_OPEN)) {
        size += ly_print(out, "/>%s", delim);
        return size;
    } else if (options & LYXML_PRINT_OPEN) {
        ly_print(out, ">");
        return ++size;
    } else if (options & LYXML_PRINT_ATTRS) {
        return size;
    }

    if (!e->child && (!e->content || !e->content[0])) {
        size += ly_print(out, "/>%s", delim);
        return size;
    } else if (e->content && e->content[0]) {
        ly_print(out, ">");
        size++;

        size += lyxml_dump_text(out, e->content, LYXML_DATA_ELEM);

        if (e->ns && e->ns->prefix) {
            size += ly_print(out, "</%s:%s>%s", e->ns->prefix, e->name, delim);
        } else {
            size += ly_print(out, "</%s>%s", e->name, delim);
        }
        return size;
    } else {
        size += ly_print(out, ">%s", delim);
    }

    /* go recursively */
    LY_TREE_FOR(e->child, child) {
        if (options & LYXML_PRINT_FORMAT) {
            size += dump_elem(out, child, level + 1, LYXML_PRINT_FORMAT, 0);
        } else {
            size += dump_elem(out, child, level, 0, 0);
        }
    }

close:
    /* closing tag */
    if (e->ns && e->ns->prefix) {
        size += ly_print(out, "%*s</%s:%s>%s", indent, "", e->ns->prefix, e->name, delim_outer);
    } else {
        size += ly_print(out, "%*s</%s>%s", indent, "", e->name, delim_outer);
    }

    return size;
}