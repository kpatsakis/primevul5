lyxml_correct_content_ns(struct ly_ctx *ctx, struct lyxml_elem *elem, struct lyxml_elem *orig)
{
    const char *end, *cur_expr;
    char *prefix;
    uint16_t i;
    size_t pref_len;
    const struct lyxml_ns *ns;
    struct lyxp_expr *exp;
    enum int_log_opts prev_ilo;

    /* it may not be a valid XPath expression */
    ly_ilo_change(NULL, ILO_IGNORE, &prev_ilo, NULL);
    exp = lyxp_parse_expr(ctx, elem->content);
    ly_ilo_restore(NULL, prev_ilo, NULL, 0);
    if (!exp) {
        goto cleanup;
    }

    for (i = 0; i < exp->used; ++i) {
        cur_expr = &exp->expr[exp->expr_pos[i]];

        if ((exp->tokens[i] == LYXP_TOKEN_NAMETEST) && (end = strnchr(cur_expr, ':', exp->tok_len[i]))) {
            /* get the prefix */
            pref_len = end - cur_expr;
            prefix = strndup(cur_expr, pref_len);
            if (!prefix) {
                LOGMEM(ctx);
                goto cleanup;
            }
            ns = lyxml_get_ns(elem, prefix);

            /* we already have the namespace */
            if (ns) {
                free(prefix);
                continue;
            }

            /* find the namespace in the original XML */
            ns = lyxml_get_ns(orig, prefix);
            free(prefix);

            /* copy the namespace over, if any */
            if (ns && !lyxml_dup_attr(ctx, elem, (struct lyxml_attr *)ns)) {
                LOGINT(ctx);
                goto cleanup;
            }
        }
    }

cleanup:
    lyxp_expr_free(exp);
}