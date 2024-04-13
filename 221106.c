read_yin_subnode(struct ly_ctx *ctx, struct lyxml_elem *node, const char *name)
{
    int len;

    /* there should be <text> child */
    if (!node->child || !node->child->name || strcmp(node->child->name, name)) {
        LOGERR(ctx, LY_EVALID, "Expected \"%s\" element in \"%s\" element.", name, node->name);
        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, name, node->name);
        return NULL;
    } else if (node->child->content) {
        len = strlen(node->child->content);
        return lydict_insert(ctx, node->child->content, len);
    } else {
        return lydict_insert(ctx, "", 0);
    }
}