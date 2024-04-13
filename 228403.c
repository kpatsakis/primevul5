lys_ext_instance_presence(struct lys_ext *def, struct lys_ext_instance **ext, uint8_t ext_size)
{
    FUN_IN;

    uint8_t index;

    if (!def || (ext_size && !ext)) {
        LOGARG;
        return -1;
    }

    /* search for the extension instance */
    for (index = 0; index < ext_size; index++) {
        if (ext[index]->module->ctx == def->module->ctx) {
            /* from the same context */
            if (ext[index]->def == def) {
                return index;
            }
        } else {
            /* from different contexts */
            if (ly_strequal0(ext[index]->def->name, def->name)
                    && ly_strequal0(lys_main_module(ext[index]->def->module)->name, lys_main_module(def->module)->name)) {
                return index;
            }
        }
    }

    /* not found */
    return -1;
}