jbig2_decode_symbol_dict(Jbig2Ctx *ctx,
                         Jbig2Segment *segment,
                         const Jbig2SymbolDictParams *params, const byte *data, size_t size, Jbig2ArithCx *GB_stats, Jbig2ArithCx *GR_stats)
{
    Jbig2SymbolDict *SDNEWSYMS = NULL;
    Jbig2SymbolDict *SDEXSYMS = NULL;
    uint32_t HCHEIGHT;
    uint32_t NSYMSDECODED;
    uint32_t SYMWIDTH, TOTWIDTH;
    uint32_t HCFIRSTSYM;
    uint32_t *SDNEWSYMWIDTHS = NULL;
    int SBSYMCODELEN = 0;
    Jbig2WordStream *ws = NULL;
    Jbig2HuffmanState *hs = NULL;
    Jbig2HuffmanTable *SDHUFFRDX = NULL;
    Jbig2HuffmanTable *SBHUFFRSIZE = NULL;
    Jbig2ArithState *as = NULL;
    Jbig2ArithIntCtx *IADH = NULL;
    Jbig2ArithIntCtx *IADW = NULL;
    Jbig2ArithIntCtx *IAEX = NULL;
    Jbig2ArithIntCtx *IAAI = NULL;
    Jbig2ArithIaidCtx *IAID = NULL;
    Jbig2ArithIntCtx *IARDX = NULL;
    Jbig2ArithIntCtx *IARDY = NULL;
    int code = 0;
    Jbig2SymbolDict **refagg_dicts = NULL;
    int n_refagg_dicts = 1;

    Jbig2TextRegionParams *tparams = NULL;

    /* 6.5.5 (3) */
    HCHEIGHT = 0;
    NSYMSDECODED = 0;

    ws = jbig2_word_stream_buf_new(ctx, data, size);
    if (ws == NULL) {
        jbig2_error(ctx, JBIG2_SEVERITY_WARNING, segment->number, "failed to allocate ws in jbig2_decode_symbol_dict");
        return NULL;
    }

    as = jbig2_arith_new(ctx, ws);
    if (as == NULL) {
        jbig2_error(ctx, JBIG2_SEVERITY_WARNING, segment->number, "failed to allocate as in jbig2_decode_symbol_dict");
        jbig2_word_stream_buf_free(ctx, ws);
        return NULL;
    }

    if (!params->SDHUFF) {
        IADH = jbig2_arith_int_ctx_new(ctx);
        IADW = jbig2_arith_int_ctx_new(ctx);
        IAEX = jbig2_arith_int_ctx_new(ctx);
        IAAI = jbig2_arith_int_ctx_new(ctx);
        if ((IADH == NULL) || (IADW == NULL) || (IAEX == NULL) || (IAAI == NULL)) {
            jbig2_error(ctx, JBIG2_SEVERITY_WARNING, segment->number, "failed to allocate storage for symbol bitmap");
            goto cleanup1;
        }
        if (params->SDREFAGG) {
            int64_t tmp = params->SDNUMINSYMS + params->SDNUMNEWSYMS;

            for (SBSYMCODELEN = 0; ((int64_t) 1 << SBSYMCODELEN) < tmp; SBSYMCODELEN++);
            IAID = jbig2_arith_iaid_ctx_new(ctx, SBSYMCODELEN);
            IARDX = jbig2_arith_int_ctx_new(ctx);
            IARDY = jbig2_arith_int_ctx_new(ctx);
            if ((IAID == NULL) || (IARDX == NULL) || (IARDY == NULL)) {
                jbig2_error(ctx, JBIG2_SEVERITY_WARNING, segment->number, "failed to allocate storage for symbol bitmap");
                goto cleanup2;
            }
        }
    } else {
        jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, segment->number, "huffman coded symbol dictionary");
        hs = jbig2_huffman_new(ctx, ws);
        SDHUFFRDX = jbig2_build_huffman_table(ctx, &jbig2_huffman_params_O);
        SBHUFFRSIZE = jbig2_build_huffman_table(ctx, &jbig2_huffman_params_A);
        if ((hs == NULL) || (SDHUFFRDX == NULL) || (SBHUFFRSIZE == NULL)) {
            jbig2_error(ctx, JBIG2_SEVERITY_WARNING, segment->number, "failed to allocate storage for symbol bitmap");
            goto cleanup2;
        }
        if (!params->SDREFAGG) {
            SDNEWSYMWIDTHS = jbig2_new(ctx, uint32_t, params->SDNUMNEWSYMS);
            if (SDNEWSYMWIDTHS == NULL) {
                jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "could not allocate storage for (%u) symbol widths", params->SDNUMNEWSYMS);
                goto cleanup2;
            }
        }
    }

    SDNEWSYMS = jbig2_sd_new(ctx, params->SDNUMNEWSYMS);
    if (SDNEWSYMS == NULL) {
        jbig2_error(ctx, JBIG2_SEVERITY_WARNING, segment->number, "could not allocate storage for (%u) new symbols", params->SDNUMNEWSYMS);
        goto cleanup2;
    }

    /* 6.5.5 (4a) */
    while (NSYMSDECODED < params->SDNUMNEWSYMS) {
        int32_t HCDH, DW;

        /* 6.5.6 */
        if (params->SDHUFF) {
            HCDH = jbig2_huffman_get(hs, params->SDHUFFDH, &code);
        } else {
            code = jbig2_arith_int_decode(IADH, as, &HCDH);
        }

        if (code != 0) {
            jbig2_error(ctx, JBIG2_SEVERITY_WARNING, segment->number, "error or OOB decoding height class delta (%d)\n", code);
        }

        if (!params->SDHUFF && jbig2_arith_has_reached_marker(as)) {
            code = jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "prevent DOS while decoding height classes");
            goto cleanup2;
        }

        /* 6.5.5 (4b) */
        HCHEIGHT = HCHEIGHT + HCDH;
        SYMWIDTH = 0;
        TOTWIDTH = 0;
        HCFIRSTSYM = NSYMSDECODED;

        if ((int32_t) HCHEIGHT < 0) {
            code = jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "Invalid HCHEIGHT value");
            goto cleanup2;
        }
#ifdef JBIG2_DEBUG
        jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, segment->number, "HCHEIGHT = %d", HCHEIGHT);
#endif
        jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, segment->number, "decoding height class %d with %d syms decoded", HCHEIGHT, NSYMSDECODED);

        for (;;) {
            /* 6.5.7 */
            if (params->SDHUFF) {
                DW = jbig2_huffman_get(hs, params->SDHUFFDW, &code);
            } else {
                code = jbig2_arith_int_decode(IADW, as, &DW);
            }
            if (code < 0)
                goto cleanup4;

            /* 6.5.5 (4c.i) */
            if (code == 1) {
                jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, segment->number, " OOB signals end of height class %d", HCHEIGHT);
                break;
            }

            /* check for broken symbol table */
            if (NSYMSDECODED >= params->SDNUMNEWSYMS) {
                jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "No OOB signalling end of height class %d", HCHEIGHT);
                goto cleanup4;
            }

            SYMWIDTH = SYMWIDTH + DW;
            TOTWIDTH = TOTWIDTH + SYMWIDTH;
            if ((int32_t) SYMWIDTH < 0) {
                code = jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "Invalid SYMWIDTH value (%d) at symbol %d", SYMWIDTH, NSYMSDECODED + 1);
                goto cleanup4;
            }
#ifdef JBIG2_DEBUG
            jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, segment->number, "SYMWIDTH = %d TOTWIDTH = %d", SYMWIDTH, TOTWIDTH);
#endif
            /* 6.5.5 (4c.ii) */
            if (!params->SDHUFF || params->SDREFAGG) {
#ifdef JBIG2_DEBUG
                jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, segment->number, "SDHUFF = %d; SDREFAGG = %d", params->SDHUFF, params->SDREFAGG);
#endif
                /* 6.5.8 */
                if (!params->SDREFAGG) {
                    Jbig2GenericRegionParams region_params;
                    int sdat_bytes;
                    Jbig2Image *image;

                    /* Table 16 */
                    region_params.MMR = 0;
                    region_params.GBTEMPLATE = params->SDTEMPLATE;
                    region_params.TPGDON = 0;
                    region_params.USESKIP = 0;
                    sdat_bytes = params->SDTEMPLATE == 0 ? 8 : 2;
                    memcpy(region_params.gbat, params->sdat, sdat_bytes);

                    image = jbig2_image_new(ctx, SYMWIDTH, HCHEIGHT);
                    if (image == NULL) {
                        code = jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "failed to allocate image in jbig2_decode_symbol_dict");
                        goto cleanup4;
                    }

                    code = jbig2_decode_generic_region(ctx, segment, &region_params, as, image, GB_stats);
                    if (code < 0) {
                        jbig2_image_release(ctx, image);
                        goto cleanup4;
                    }

                    SDNEWSYMS->glyphs[NSYMSDECODED] = image;
                } else {
                    /* 6.5.8.2 refinement/aggregate symbol */
                    uint32_t REFAGGNINST;

                    if (params->SDHUFF) {
                        REFAGGNINST = jbig2_huffman_get(hs, params->SDHUFFAGGINST, &code);
                    } else {
                        code = jbig2_arith_int_decode(IAAI, as, (int32_t *) & REFAGGNINST);
                    }
                    if (code || (int32_t) REFAGGNINST <= 0) {
                        code = jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "invalid number of symbols or OOB in aggregate glyph");
                        goto cleanup4;
                    }

                    jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, segment->number, "aggregate symbol coding (%d instances)", REFAGGNINST);

                    if (REFAGGNINST > 1) {
                        Jbig2Image *image;
                        uint32_t i;

                        if (tparams == NULL) {
                            /* First time through, we need to initialise the */
                            /* various tables for Huffman or adaptive encoding */
                            /* as well as the text region parameters structure */
                            refagg_dicts = jbig2_new(ctx, Jbig2SymbolDict *, n_refagg_dicts);
                            if (refagg_dicts == NULL) {
                                code = jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "Out of memory allocating dictionary array");
                                goto cleanup4;
                            }
                            refagg_dicts[0] = jbig2_sd_new(ctx, params->SDNUMINSYMS + params->SDNUMNEWSYMS);
                            if (refagg_dicts[0] == NULL) {
                                code = jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "Out of memory allocating symbol dictionary");
                                jbig2_free(ctx->allocator, refagg_dicts);
                                goto cleanup4;
                            }
                            for (i = 0; i < params->SDNUMINSYMS; i++) {
                                refagg_dicts[0]->glyphs[i] = jbig2_image_clone(ctx, params->SDINSYMS->glyphs[i]);
                            }

                            tparams = jbig2_new(ctx, Jbig2TextRegionParams, 1);
                            if (tparams == NULL) {
                                code = jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "Out of memory creating text region params");
                                goto cleanup4;
                            }
                            if (!params->SDHUFF) {
                                /* Values from Table 17, section 6.5.8.2 (2) */
                                tparams->IADT = jbig2_arith_int_ctx_new(ctx);
                                tparams->IAFS = jbig2_arith_int_ctx_new(ctx);
                                tparams->IADS = jbig2_arith_int_ctx_new(ctx);
                                tparams->IAIT = jbig2_arith_int_ctx_new(ctx);
                                /* Table 31 */
                                for (SBSYMCODELEN = 0; (1 << SBSYMCODELEN) < (int)(params->SDNUMINSYMS + params->SDNUMNEWSYMS); SBSYMCODELEN++);
                                tparams->IAID = jbig2_arith_iaid_ctx_new(ctx, SBSYMCODELEN);
                                tparams->IARI = jbig2_arith_int_ctx_new(ctx);
                                tparams->IARDW = jbig2_arith_int_ctx_new(ctx);
                                tparams->IARDH = jbig2_arith_int_ctx_new(ctx);
                                tparams->IARDX = jbig2_arith_int_ctx_new(ctx);
                                tparams->IARDY = jbig2_arith_int_ctx_new(ctx);
                            } else {
                                tparams->SBHUFFFS = jbig2_build_huffman_table(ctx, &jbig2_huffman_params_F);    /* Table B.6 */
                                tparams->SBHUFFDS = jbig2_build_huffman_table(ctx, &jbig2_huffman_params_H);    /* Table B.8 */
                                tparams->SBHUFFDT = jbig2_build_huffman_table(ctx, &jbig2_huffman_params_K);    /* Table B.11 */
                                tparams->SBHUFFRDW = jbig2_build_huffman_table(ctx, &jbig2_huffman_params_O);   /* Table B.15 */
                                tparams->SBHUFFRDH = jbig2_build_huffman_table(ctx, &jbig2_huffman_params_O);   /* Table B.15 */
                                tparams->SBHUFFRDX = jbig2_build_huffman_table(ctx, &jbig2_huffman_params_O);   /* Table B.15 */
                                tparams->SBHUFFRDY = jbig2_build_huffman_table(ctx, &jbig2_huffman_params_O);   /* Table B.15 */
                            }
                            tparams->SBHUFF = params->SDHUFF;
                            tparams->SBREFINE = 1;
                            tparams->SBSTRIPS = 1;
                            tparams->SBDEFPIXEL = 0;
                            tparams->SBCOMBOP = JBIG2_COMPOSE_OR;
                            tparams->TRANSPOSED = 0;
                            tparams->REFCORNER = JBIG2_CORNER_TOPLEFT;
                            tparams->SBDSOFFSET = 0;
                            tparams->SBRTEMPLATE = params->SDRTEMPLATE;
                        }
                        tparams->SBNUMINSTANCES = REFAGGNINST;

                        image = jbig2_image_new(ctx, SYMWIDTH, HCHEIGHT);
                        if (image == NULL) {
                            code = jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "Out of memory creating symbol image");
                            goto cleanup4;
                        }

                        /* multiple symbols are handled as a text region */
                        jbig2_decode_text_region(ctx, segment, tparams, (const Jbig2SymbolDict * const *)refagg_dicts,
                                                 n_refagg_dicts, image, data, size, GR_stats, as, ws);

                        SDNEWSYMS->glyphs[NSYMSDECODED] = image;
                        refagg_dicts[0]->glyphs[params->SDNUMINSYMS + NSYMSDECODED] = jbig2_image_clone(ctx, SDNEWSYMS->glyphs[NSYMSDECODED]);
                    } else {
                        /* 6.5.8.2.2 */
                        /* bool SBHUFF = params->SDHUFF; */
                        Jbig2RefinementRegionParams rparams;
                        Jbig2Image *image;
                        uint32_t ID;
                        int32_t RDX, RDY;
                        int BMSIZE = 0;
                        uint32_t ninsyms = params->SDNUMINSYMS;
                        int code1 = 0;
                        int code2 = 0;
                        int code3 = 0;
                        int code4 = 0;

                        /* 6.5.8.2.2 (2, 3, 4, 5) */
                        if (params->SDHUFF) {
                            ID = jbig2_huffman_get_bits(hs, SBSYMCODELEN, &code4);
                            RDX = jbig2_huffman_get(hs, SDHUFFRDX, &code1);
                            RDY = jbig2_huffman_get(hs, SDHUFFRDX, &code2);
                            BMSIZE = jbig2_huffman_get(hs, SBHUFFRSIZE, &code3);
                            jbig2_huffman_skip(hs);
                        } else {
                            code1 = jbig2_arith_iaid_decode(IAID, as, (int32_t *) & ID);
                            code2 = jbig2_arith_int_decode(IARDX, as, &RDX);
                            code3 = jbig2_arith_int_decode(IARDY, as, &RDY);
                        }

                        if ((code1 < 0) || (code2 < 0) || (code3 < 0) || (code4 < 0)) {
                            code = jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "failed to decode data");
                            goto cleanup4;
                        }

                        if (ID >= ninsyms + NSYMSDECODED) {
                            code = jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "refinement references unknown symbol %d", ID);
                            goto cleanup4;
                        }

                        jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, segment->number,
                                    "symbol is a refinement of id %d with the " "refinement applied at (%d,%d)", ID, RDX, RDY);

                        image = jbig2_image_new(ctx, SYMWIDTH, HCHEIGHT);
                        if (image == NULL) {
                            code = jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "Out of memory creating symbol image");
                            goto cleanup4;
                        }

                        /* Table 18 */
                        rparams.GRTEMPLATE = params->SDRTEMPLATE;
                        rparams.reference = (ID < ninsyms) ? params->SDINSYMS->glyphs[ID] : SDNEWSYMS->glyphs[ID - ninsyms];
                        /* SumatraPDF: fail on missing glyphs */
                        if (rparams.reference == NULL) {
                            code = jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "missing glyph %d/%d!", ID, ninsyms);
                            jbig2_image_release(ctx, image);
                            goto cleanup4;
                        }
                        rparams.DX = RDX;
                        rparams.DY = RDY;
                        rparams.TPGRON = 0;
                        memcpy(rparams.grat, params->sdrat, 4);
                        code = jbig2_decode_refinement_region(ctx, segment, &rparams, as, image, GR_stats);
                        if (code < 0)
                            goto cleanup4;

                        SDNEWSYMS->glyphs[NSYMSDECODED] = image;

                        /* 6.5.8.2.2 (7) */
                        if (params->SDHUFF) {
                            if (BMSIZE == 0)
                                BMSIZE = image->height * image->stride;
                            jbig2_huffman_advance(hs, BMSIZE);
                        }
                    }
                }

#ifdef OUTPUT_PBM
                {
                    char name[64];
                    FILE *out;

                    snprintf(name, 64, "sd.%04d.%04d.pbm", segment->number, NSYMSDECODED);
                    out = fopen(name, "wb");
                    jbig2_image_write_pbm(SDNEWSYMS->glyphs[NSYMSDECODED], out);
                    jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, segment->number, "writing out glyph as '%s' ...", name);
                    fclose(out);
                }
#endif

            }

            /* 6.5.5 (4c.iii) */
            if (params->SDHUFF && !params->SDREFAGG) {
                SDNEWSYMWIDTHS[NSYMSDECODED] = SYMWIDTH;
            }

            /* 6.5.5 (4c.iv) */
            NSYMSDECODED = NSYMSDECODED + 1;

            jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, segment->number, "decoded symbol %u of %u (%ux%u)", NSYMSDECODED, params->SDNUMNEWSYMS, SYMWIDTH, HCHEIGHT);

        }                       /* end height class decode loop */

        /* 6.5.5 (4d) */
        if (params->SDHUFF && !params->SDREFAGG) {
            /* 6.5.9 */
            Jbig2Image *image;
            uint32_t BMSIZE = jbig2_huffman_get(hs, params->SDHUFFBMSIZE, &code);
            uint32_t j;
            int x;

            if (code) {
                jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "error decoding size of collective bitmap!");
                goto cleanup4;
            }

            /* skip any bits before the next byte boundary */
            jbig2_huffman_skip(hs);

            image = jbig2_image_new(ctx, TOTWIDTH, HCHEIGHT);
            if (image == NULL) {
                jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "could not allocate collective bitmap image!");
                goto cleanup4;
            }

            if (BMSIZE == 0) {
                /* if BMSIZE == 0 bitmap is uncompressed */
                const byte *src = data + jbig2_huffman_offset(hs);
                const int stride = (image->width >> 3) + ((image->width & 7) ? 1 : 0);
                byte *dst = image->data;

                /* SumatraPDF: prevent read access violation */
                if ((size - jbig2_huffman_offset(hs) < image->height * stride) || (size < jbig2_huffman_offset(hs))) {
                    jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "not enough data for decoding (%d/%d)", image->height * stride,
                                size - jbig2_huffman_offset(hs));
                    jbig2_image_release(ctx, image);
                    goto cleanup4;
                }

                BMSIZE = image->height * stride;
                jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, segment->number,
                            "reading %dx%d uncompressed bitmap" " for %d symbols (%d bytes)", image->width, image->height, NSYMSDECODED - HCFIRSTSYM, BMSIZE);

                for (j = 0; j < image->height; j++) {
                    memcpy(dst, src, stride);
                    dst += image->stride;
                    src += stride;
                }
            } else {
                Jbig2GenericRegionParams rparams;

                /* SumatraPDF: prevent read access violation */
                if (size - jbig2_huffman_offset(hs) < BMSIZE) {
                    jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "not enough data for decoding (%d/%d)", BMSIZE, size - jbig2_huffman_offset(hs));
                    jbig2_image_release(ctx, image);
                    goto cleanup4;
                }

                jbig2_error(ctx, JBIG2_SEVERITY_DEBUG, segment->number,
                            "reading %dx%d collective bitmap for %d symbols (%d bytes)", image->width, image->height, NSYMSDECODED - HCFIRSTSYM, BMSIZE);

                rparams.MMR = 1;
                code = jbig2_decode_generic_mmr(ctx, segment, &rparams, data + jbig2_huffman_offset(hs), BMSIZE, image);
                if (code) {
                    jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "error decoding MMR bitmap image!");
                    jbig2_image_release(ctx, image);
                    goto cleanup4;
                }
            }

            /* advance past the data we've just read */
            jbig2_huffman_advance(hs, BMSIZE);

            /* copy the collective bitmap into the symbol dictionary */
            x = 0;
            for (j = HCFIRSTSYM; j < NSYMSDECODED; j++) {
                Jbig2Image *glyph;

                glyph = jbig2_image_new(ctx, SDNEWSYMWIDTHS[j], HCHEIGHT);
                if (glyph == NULL) {
                    jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "failed to copy the collective bitmap into symbol dictionary");
                    jbig2_image_release(ctx, image);
                    goto cleanup4;
                }
                jbig2_image_compose(ctx, glyph, image, -x, 0, JBIG2_COMPOSE_REPLACE);
                x += SDNEWSYMWIDTHS[j];
                SDNEWSYMS->glyphs[j] = glyph;
            }
            jbig2_image_release(ctx, image);
        }

    }                           /* end of symbol decode loop */

    /* 6.5.10 */
    SDEXSYMS = jbig2_sd_new(ctx, params->SDNUMEXSYMS);
    if (SDEXSYMS == NULL) {
        jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "failed to allocate symbols exported from symbols dictionary");
        goto cleanup4;
    } else {
        uint32_t i = 0;
        uint32_t j = 0;
        uint32_t k;
        int exflag = 0;
        uint32_t limit = params->SDNUMINSYMS + params->SDNUMNEWSYMS;
        uint32_t exrunlength;
        int zerolength = 0;

        while (i < limit) {
            if (params->SDHUFF)
                exrunlength = jbig2_huffman_get(hs, SBHUFFRSIZE, &code);
            else
                code = jbig2_arith_int_decode(IAEX, as, (int32_t *)&exrunlength);
            /* prevent infinite loop */
            zerolength = exrunlength > 0 ? 0 : zerolength + 1;
            if (code || (exrunlength > limit - i) || (zerolength > 4) || (exflag && (exrunlength + j > params->SDNUMEXSYMS))) {
                if (code)
                    jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "failed to decode exrunlength for exported symbols");
                else if (exrunlength <= 0)
                    jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number, "runlength too small in export symbol table (%d <= 0)\n", exrunlength);
                else
                    jbig2_error(ctx, JBIG2_SEVERITY_FATAL, segment->number,
                                "runlength too large in export symbol table (%d > %d - %d)\n", exrunlength, params->SDNUMEXSYMS, j);
                /* skip to the cleanup code and return SDEXSYMS = NULL */
                jbig2_sd_release(ctx, SDEXSYMS);
                SDEXSYMS = NULL;
                break;
            }
            for (k = 0; k < exrunlength; k++) {
                if (exflag) {
                    SDEXSYMS->glyphs[j++] = (i < params->SDNUMINSYMS) ?
                                            jbig2_image_clone(ctx, params->SDINSYMS->glyphs[i]) : jbig2_image_clone(ctx, SDNEWSYMS->glyphs[i - params->SDNUMINSYMS]);
                }
                i++;
            }
            exflag = !exflag;
        }
    }

cleanup4:
    if (tparams != NULL) {
        if (!params->SDHUFF) {
            jbig2_arith_int_ctx_free(ctx, tparams->IADT);
            jbig2_arith_int_ctx_free(ctx, tparams->IAFS);
            jbig2_arith_int_ctx_free(ctx, tparams->IADS);
            jbig2_arith_int_ctx_free(ctx, tparams->IAIT);
            jbig2_arith_iaid_ctx_free(ctx, tparams->IAID);
            jbig2_arith_int_ctx_free(ctx, tparams->IARI);
            jbig2_arith_int_ctx_free(ctx, tparams->IARDW);
            jbig2_arith_int_ctx_free(ctx, tparams->IARDH);
            jbig2_arith_int_ctx_free(ctx, tparams->IARDX);
            jbig2_arith_int_ctx_free(ctx, tparams->IARDY);
        } else {
            jbig2_release_huffman_table(ctx, tparams->SBHUFFFS);
            jbig2_release_huffman_table(ctx, tparams->SBHUFFDS);
            jbig2_release_huffman_table(ctx, tparams->SBHUFFDT);
            jbig2_release_huffman_table(ctx, tparams->SBHUFFRDX);
            jbig2_release_huffman_table(ctx, tparams->SBHUFFRDY);
            jbig2_release_huffman_table(ctx, tparams->SBHUFFRDW);
            jbig2_release_huffman_table(ctx, tparams->SBHUFFRDH);
        }
        jbig2_free(ctx->allocator, tparams);
    }
    if (refagg_dicts != NULL) {
        jbig2_sd_release(ctx, refagg_dicts[0]);
        jbig2_free(ctx->allocator, refagg_dicts);
    }

cleanup2:
    jbig2_sd_release(ctx, SDNEWSYMS);
    if (params->SDHUFF && !params->SDREFAGG) {
        jbig2_free(ctx->allocator, SDNEWSYMWIDTHS);
    }
    jbig2_release_huffman_table(ctx, SDHUFFRDX);
    jbig2_release_huffman_table(ctx, SBHUFFRSIZE);
    jbig2_huffman_free(ctx, hs);
    jbig2_arith_iaid_ctx_free(ctx, IAID);
    jbig2_arith_int_ctx_free(ctx, IARDX);
    jbig2_arith_int_ctx_free(ctx, IARDY);

cleanup1:
    jbig2_word_stream_buf_free(ctx, ws);
    jbig2_free(ctx->allocator, as);
    jbig2_arith_int_ctx_free(ctx, IADH);
    jbig2_arith_int_ctx_free(ctx, IADW);
    jbig2_arith_int_ctx_free(ctx, IAEX);
    jbig2_arith_int_ctx_free(ctx, IAAI);

    return SDEXSYMS;
}