lys_ext_complex_get_substmt(LY_STMT stmt, struct lys_ext_instance_complex *ext, struct lyext_substmt **info)
{
    FUN_IN;

    int i;

    if (!ext || !ext->def || !ext->def->plugin || ext->def->plugin->type != LYEXT_COMPLEX) {
        LOGARG;
        return NULL;
    }

    if (!ext->substmt) {
        /* no substatement defined in the plugin */
        if (info) {
            *info = NULL;
        }
        return NULL;
    }

    /* search the substatements defined by the plugin */
    for (i = 0; ext->substmt[i].stmt; i++) {
        if (stmt == LY_STMT_NODE) {
            if (ext->substmt[i].stmt >= LY_STMT_ACTION && ext->substmt[i].stmt <= LY_STMT_USES) {
                if (info) {
                    *info = &ext->substmt[i];
                }
                break;
            }
        } else if (ext->substmt[i].stmt == stmt) {
            if (info) {
                *info = &ext->substmt[i];
            }
            break;
        }
    }

    if (ext->substmt[i].stmt) {
        return &ext->content[ext->substmt[i].offset];
    } else {
        return NULL;
    }
}