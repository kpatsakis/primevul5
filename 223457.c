lyxml_dup(struct ly_ctx *ctx, struct lyxml_elem *root)
{
    FUN_IN;

    return lyxml_dup_elem(ctx, root, NULL, 1, 0);
}