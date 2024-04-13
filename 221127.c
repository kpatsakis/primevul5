yin_getplace_for_extcomplex_node(struct lyxml_elem *node, struct lys_ext_instance_complex *ext, LY_STMT stmt)
{
    struct lyext_substmt *info;
    struct lys_node **snode, *siter;

    snode = lys_ext_complex_get_substmt(stmt, ext, &info);
    if (!snode) {
        LOGVAL(ext->module->ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, node->name, node->parent->name);
        return NULL;
    }
    if (info->cardinality < LY_STMT_CARD_SOME) {
        LY_TREE_FOR(*snode, siter) {
            if (stmt == lys_snode2stmt(siter->nodetype)) {
                LOGVAL(ext->module->ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, node->name, node->parent->name);
                return NULL;
            }
        }
    }

    return snode;
}