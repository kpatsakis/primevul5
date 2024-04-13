lyxml_unlink(struct ly_ctx *ctx, struct lyxml_elem *elem)
{
    FUN_IN;

    if (!elem) {
        return;
    }

    lyxml_unlink_elem(ctx, elem, 1);
}