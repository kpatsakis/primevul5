int X509_verify_cert(X509_STORE_CTX *ctx)
{
    X509 *x, *xtmp, *xtmp2, *chain_ss = NULL;
    int bad_chain = 0;
    X509_VERIFY_PARAM *param = ctx->param;
    int depth, i, ok = 0;
    int num, j, retry;
    int (*cb) (int xok, X509_STORE_CTX *xctx);
    STACK_OF(X509) *sktmp = NULL;
    if (ctx->cert == NULL) {
        X509err(X509_F_X509_VERIFY_CERT, X509_R_NO_CERT_SET_FOR_US_TO_VERIFY);
        return -1;
    }
    if (ctx->chain != NULL) {
        /*
         * This X509_STORE_CTX has already been used to verify a cert. We
         * cannot do another one.
         */
        X509err(X509_F_X509_VERIFY_CERT, ERR_R_SHOULD_NOT_HAVE_BEEN_CALLED);
        return -1;
    }

    cb = ctx->verify_cb;

    /*
     * first we make sure the chain we are going to build is present and that
     * the first entry is in place
     */
    if (((ctx->chain = sk_X509_new_null()) == NULL) ||
        (!sk_X509_push(ctx->chain, ctx->cert))) {
        X509err(X509_F_X509_VERIFY_CERT, ERR_R_MALLOC_FAILURE);
        goto end;
    }
    CRYPTO_add(&ctx->cert->references, 1, CRYPTO_LOCK_X509);
    ctx->last_untrusted = 1;

    /* We use a temporary STACK so we can chop and hack at it */
    if (ctx->untrusted != NULL
        && (sktmp = sk_X509_dup(ctx->untrusted)) == NULL) {
        X509err(X509_F_X509_VERIFY_CERT, ERR_R_MALLOC_FAILURE);
        goto end;
    }

    num = sk_X509_num(ctx->chain);
    x = sk_X509_value(ctx->chain, num - 1);
    depth = param->depth;

    for (;;) {
        /* If we have enough, we break */
        if (depth < num)
            break;              /* FIXME: If this happens, we should take
                                 * note of it and, if appropriate, use the
                                 * X509_V_ERR_CERT_CHAIN_TOO_LONG error code
                                 * later. */

        /* If we are self signed, we break */
        if (ctx->check_issued(ctx, x, x))
            break;

        /* If we were passed a cert chain, use it first */
        if (ctx->untrusted != NULL) {
            xtmp = find_issuer(ctx, sktmp, x);
            if (xtmp != NULL) {
                if (!sk_X509_push(ctx->chain, xtmp)) {
                    X509err(X509_F_X509_VERIFY_CERT, ERR_R_MALLOC_FAILURE);
                    goto end;
                }
                CRYPTO_add(&xtmp->references, 1, CRYPTO_LOCK_X509);
                (void)sk_X509_delete_ptr(sktmp, xtmp);
                ctx->last_untrusted++;
                x = xtmp;
                num++;
                /*
                 * reparse the full chain for the next one
                 */
                continue;
            }
        }
        break;
    }

    /* Remember how many untrusted certs we have */
    j = num;
    /*
     * at this point, chain should contain a list of untrusted certificates.
     * We now need to add at least one trusted one, if possible, otherwise we
     * complain.
     */

    do {
        /*
         * Examine last certificate in chain and see if it is self signed.
         */
        i = sk_X509_num(ctx->chain);
        x = sk_X509_value(ctx->chain, i - 1);
        if (ctx->check_issued(ctx, x, x)) {
            /* we have a self signed certificate */
            if (sk_X509_num(ctx->chain) == 1) {
                /*
                 * We have a single self signed certificate: see if we can
                 * find it in the store. We must have an exact match to avoid
                 * possible impersonation.
                 */
                ok = ctx->get_issuer(&xtmp, ctx, x);
                if ((ok <= 0) || X509_cmp(x, xtmp)) {
                    ctx->error = X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT;
                    ctx->current_cert = x;
                    ctx->error_depth = i - 1;
                    if (ok == 1)
                        X509_free(xtmp);
                    bad_chain = 1;
                    ok = cb(0, ctx);
                    if (!ok)
                        goto end;
                } else {
                    /*
                     * We have a match: replace certificate with store
                     * version so we get any trust settings.
                     */
                    X509_free(x);
                    x = xtmp;
                    (void)sk_X509_set(ctx->chain, i - 1, x);
                    ctx->last_untrusted = 0;
                }
            } else {
                /*
                 * extract and save self signed certificate for later use
                 */
                chain_ss = sk_X509_pop(ctx->chain);
                ctx->last_untrusted--;
                num--;
                j--;
                x = sk_X509_value(ctx->chain, num - 1);
            }
        }
        /* We now lookup certs from the certificate store */
        for (;;) {
            /* If we have enough, we break */
            if (depth < num)
                break;
            /* If we are self signed, we break */
            if (ctx->check_issued(ctx, x, x))
                break;
            ok = ctx->get_issuer(&xtmp, ctx, x);
            if (ok < 0)
                return ok;
            if (ok == 0)
                break;
            x = xtmp;
            if (!sk_X509_push(ctx->chain, x)) {
                X509_free(xtmp);
                X509err(X509_F_X509_VERIFY_CERT, ERR_R_MALLOC_FAILURE);
                return 0;
            }
            num++;
        }

        /*
         * If we haven't got a least one certificate from our store then check
         * if there is an alternative chain that could be used.  We only do this
         * if the user hasn't switched off alternate chain checking
         */
        retry = 0;
        if (num == ctx->last_untrusted &&
            !(ctx->param->flags & X509_V_FLAG_NO_ALT_CHAINS)) {
            while (j-- > 1) {
                xtmp2 = sk_X509_value(ctx->chain, j - 1);
                ok = ctx->get_issuer(&xtmp, ctx, xtmp2);
                if (ok < 0)
                    goto end;
                /* Check if we found an alternate chain */
                if (ok > 0) {
                    /*
                     * Free up the found cert we'll add it again later
                     */
                    X509_free(xtmp);

                    /*
                     * Dump all the certs above this point - we've found an
                     * alternate chain
                     */
                    while (num > j) {
                        xtmp = sk_X509_pop(ctx->chain);
                        X509_free(xtmp);
                        num--;
                    }
                    ctx->last_untrusted = sk_X509_num(ctx->chain);
                    retry = 1;
                    break;
                }
            }
        }
    } while (retry);

    /* Is last certificate looked up self signed? */
    if (!ctx->check_issued(ctx, x, x)) {
        if ((chain_ss == NULL) || !ctx->check_issued(ctx, x, chain_ss)) {
            if (ctx->last_untrusted >= num)
                ctx->error = X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY;
            else
                ctx->error = X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT;
            ctx->current_cert = x;
        } else {

            sk_X509_push(ctx->chain, chain_ss);
            num++;
            ctx->last_untrusted = num;
            ctx->current_cert = chain_ss;
            ctx->error = X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN;
            chain_ss = NULL;
        }

        ctx->error_depth = num - 1;
        bad_chain = 1;
        ok = cb(0, ctx);
        if (!ok)
            goto end;
    }

    /* We have the chain complete: now we need to check its purpose */
    ok = check_chain_extensions(ctx);

    if (!ok)
        goto end;

    /* Check name constraints */

    ok = check_name_constraints(ctx);

    if (!ok)
        goto end;

    /* The chain extensions are OK: check trust */

    if (param->trust > 0)
        ok = check_trust(ctx);

    if (!ok)
        goto end;

    /* We may as well copy down any DSA parameters that are required */
    X509_get_pubkey_parameters(NULL, ctx->chain);

    /*
     * Check revocation status: we do this after copying parameters because
     * they may be needed for CRL signature verification.
     */

    ok = ctx->check_revocation(ctx);
    if (!ok)
        goto end;

    /* At this point, we have a chain and need to verify it */
    if (ctx->verify != NULL)
        ok = ctx->verify(ctx);
    else
        ok = internal_verify(ctx);
    if (!ok)
        goto end;

#ifndef OPENSSL_NO_RFC3779
    /* RFC 3779 path validation, now that CRL check has been done */
    ok = v3_asid_validate_path(ctx);
    if (!ok)
        goto end;
    ok = v3_addr_validate_path(ctx);
    if (!ok)
        goto end;
#endif

    /* If we get this far evaluate policies */
    if (!bad_chain && (ctx->param->flags & X509_V_FLAG_POLICY_CHECK))
        ok = ctx->check_policy(ctx);
    if (!ok)
        goto end;
    if (0) {
 end:
        X509_get_pubkey_parameters(NULL, ctx->chain);
    }
    if (sktmp != NULL)
        sk_X509_free(sktmp);
    if (chain_ss != NULL)
        X509_free(chain_ss);
    return ok;
}