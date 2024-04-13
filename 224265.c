static Token *pp_tokline(void)
{
    while (true) {
        Line *l = istk->expansion;
        Token *tline = NULL;
        Token *dtline;

        /*
         * Fetch a tokenized line, either from the macro-expansion
         * buffer or from the input file.
         */
        tline = NULL;
        while (l && l->finishes) {
            MMacro *fm = l->finishes;

            nasm_assert(fm == istk->mstk.mstk);

            if (!fm->name && fm->in_progress > 1) {
                /*
                 * This is a macro-end marker for a macro with no
                 * name, which means it's not really a macro at all
                 * but a %rep block, and the `in_progress' field is
                 * more than 1, meaning that we still need to
                 * repeat. (1 means the natural last repetition; 0
                 * means termination by %exitrep.) We have
                 * therefore expanded up to the %endrep, and must
                 * push the whole block on to the expansion buffer
                 * again. We don't bother to remove the macro-end
                 * marker: we'd only have to generate another one
                 * if we did.
                 */
                fm->in_progress--;
                list_for_each(l, fm->expansion) {
                    Line *ll;

                    nasm_new(ll);
                    ll->next  = istk->expansion;
                    ll->first = dup_tlist(l->first, NULL);
                    ll->where = l->where;
                    istk->expansion = ll;
                }
                break;
            } else {
                MMacro *m = istk->mstk.mstk;

                /*
                 * Check whether a `%rep' was started and not ended
                 * within this macro expansion. This can happen and
                 * should be detected. It's a fatal error because
                 * I'm too confused to work out how to recover
                 * sensibly from it.
                 */
                if (defining) {
                    if (defining->name)
                        nasm_panic("defining with name in expansion");
                    else if (m->name)
                        nasm_fatal("`%%rep' without `%%endrep' within"
				   " expansion of macro `%s'", m->name);
                }

                /*
                 * FIXME:  investigate the relationship at this point between
                 * istk->mstk.mstk and fm
                 */
                istk->mstk = m->mstk;
                if (m->name) {
                    /*
                     * This was a real macro call, not a %rep, and
                     * therefore the parameter information needs to
                     * be freed and the iteration count/nesting
                     * depth adjusted.
                     */

                    if (!--mmacro_deadman.levels) {
                        /*
                         * If all mmacro processing done,
                         * clear all counters and the deadman
                         * message trigger.
                         */
                        nasm_zero(mmacro_deadman); /* Clear all counters */
                    }

#if 0
                    if (m->prev) {
                        pop_mmacro(m);
                        fm->in_progress --;
                    } else
#endif
                    {
                        nasm_free(m->params);
                        free_tlist(m->iline);
                        nasm_free(m->paramlen);
                        fm->in_progress = 0;
			m->params = NULL;
			m->iline = NULL;
			m->paramlen = NULL;
                    }
                }

                if (fm->nolist & NL_LIST) {
                    istk->nolist--;
                } else if (!istk->nolist) {
                    lfmt->downlevel(LIST_MACRO);
                }

                if (fm->nolist & NL_LINE) {
                    istk->noline--;
                } else if (!istk->noline) {
                    if (fm == src_macro_current())
                        src_macro_pop();
                    src_update(l->where);
                }

                istk->where = l->where;

                /*
                 * FIXME It is incorrect to always free_mmacro here.
                 * It leads to usage-after-free.
                 *
                 * https://bugzilla.nasm.us/show_bug.cgi?id=3392414
                 */
#if 0
                else
                    free_mmacro(m);
#endif
            }
            istk->expansion = l->next;
            nasm_free(l);

            return &tok_pop;
        }

        do {                    /* until we get a line we can use */
            char *line;

            if (istk->expansion) {      /* from a macro expansion */
                Line *l = istk->expansion;

                istk->expansion = l->next;
                istk->where = l->where;
                tline = l->first;
                nasm_free(l);

                if (!istk->noline)
                    src_update(istk->where);

                if (!istk->nolist) {
                    line = detoken(tline, false);
                    lfmt->line(LIST_MACRO, istk->where.lineno, line);
                    nasm_free(line);
                }
            } else if ((line = read_line())) {
                line = prepreproc(line);
                tline = tokenize(line);
                nasm_free(line);
            } else {
                /*
                 * The current file has ended; work down the istk
                 */
                Include *i = istk;
                Include *is;

                if (i->fp)
                    fclose(i->fp);
                if (i->conds) {
                    /* nasm_fatal can't be conditionally suppressed */
                    nasm_fatal("expected `%%endif' before end of file");
                }

                list_for_each(is, i->next) {
                    if (is->fp) {
                        lfmt->downlevel(LIST_INCLUDE);
                        src_update(is->where);
                        break;
                    }
                }
                istk = i->next;
                nasm_free(i);
                return &tok_pop;
            }
        } while (0);

        /*
         * We must expand MMacro parameters and MMacro-local labels
         * _before_ we plunge into directive processing, to cope
         * with things like `%define something %1' such as STRUC
         * uses. Unless we're _defining_ a MMacro, in which case
         * those tokens should be left alone to go into the
         * definition; and unless we're in a non-emitting
         * condition, in which case we don't want to meddle with
         * anything.
         */
        if (!defining &&
            !(istk->conds && !emitting(istk->conds->state)) &&
            !(istk->mstk.mmac && !istk->mstk.mmac->in_progress)) {
            tline = expand_mmac_params(tline);
        }

        /*
         * Check the line to see if it's a preprocessor directive.
         */
        if (do_directive(tline, &dtline) == DIRECTIVE_FOUND) {
            if (dtline)
                return dtline;
        } else if (defining) {
            /*
             * We're defining a multi-line macro. We emit nothing
             * at all, and just
             * shove the tokenized line on to the macro definition.
             */
            MMacro *mmac = defining->dstk.mmac;
            Line *l;

            nasm_new(l);
            l->next = defining->expansion;
            l->first = tline;
            l->finishes = NULL;
            l->where = istk->where;
            defining->expansion = l;

            /*
             * Remember if this mmacro expansion contains %00:
             * if it does, we will have to handle leading labels
             * specially.
             */
            if (mmac) {
                const Token *t;
                list_for_each(t, tline) {
                    if (!memcmp(t->text.a, "%00", 4))
                        mmac->capture_label = true;
                }
            }
        } else if (istk->conds && !emitting(istk->conds->state)) {
            /*
             * We're in a non-emitting branch of a condition block.
             * Emit nothing at all, not even a blank line: when we
             * emerge from the condition we'll give a line-number
             * directive so we keep our place correctly.
             */
            free_tlist(tline);
        } else if (istk->mstk.mstk && !istk->mstk.mstk->in_progress) {
            /*
             * We're in a %rep block which has been terminated, so
             * we're walking through to the %endrep without
             * emitting anything. Emit nothing at all, not even a
             * blank line: when we emerge from the %rep block we'll
             * give a line-number directive so we keep our place
             * correctly.
             */
            free_tlist(tline);
        } else {
            tline = expand_smacro(tline);
            if (!expand_mmacro(tline))
                return tline;
        }
    }
}