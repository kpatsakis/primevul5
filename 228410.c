lys_snode2stmt(LYS_NODE nodetype)
{
    switch(nodetype) {
    case LYS_CONTAINER:
        return LY_STMT_CONTAINER;
    case LYS_CHOICE:
        return LY_STMT_CHOICE;
    case LYS_LEAF:
        return LY_STMT_LEAF;
    case LYS_LEAFLIST:
        return LY_STMT_LEAFLIST;
    case LYS_LIST:
        return LY_STMT_LIST;
    case LYS_ANYXML:
    case LYS_ANYDATA:
        return LY_STMT_ANYDATA;
    case LYS_CASE:
        return LY_STMT_CASE;
    case LYS_NOTIF:
        return LY_STMT_NOTIFICATION;
    case LYS_RPC:
        return LY_STMT_RPC;
    case LYS_INPUT:
        return LY_STMT_INPUT;
    case LYS_OUTPUT:
        return LY_STMT_OUTPUT;
    case LYS_GROUPING:
        return LY_STMT_GROUPING;
    case LYS_USES:
        return LY_STMT_USES;
    case LYS_AUGMENT:
        return LY_STMT_AUGMENT;
    case LYS_ACTION:
        return LY_STMT_ACTION;
    default:
        return LY_STMT_NODE;
    }
}