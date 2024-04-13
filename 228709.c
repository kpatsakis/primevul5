static void SFDDumpChar(FILE *sfd,SplineChar *sc,EncMap *map,int *newgids,int todir,int saveUndoes) {
    // TODO: Output the U. F. O. glif name.
    ImageList *img;
    KernPair *kp;
    PST *pst;
    int i, v, enc;
    struct altuni *altuni;

    if (!todir)
	putc('\n',sfd);

    SFDDumpCharStartingMarker( sfd, sc );
    if ( (enc = map->backmap[sc->orig_pos])>=map->enccount ) {
	if ( sc->parent->cidmaster==NULL )
	    IError("Bad reverse encoding");
	enc = -1;
    }
    if ( sc->unicodeenc!=-1 &&
	    ((map->enc->is_unicodebmp && sc->unicodeenc<0x10000) ||
	     (map->enc->is_unicodefull && sc->unicodeenc < (int)unicode4_size)) )
	/* If we have altunis, then the backmap may not give the primary */
	/*  unicode code point, which is what we need here */
	fprintf(sfd, "Encoding: %d %d %d\n", sc->unicodeenc, sc->unicodeenc,
		newgids!=NULL?newgids[sc->orig_pos]:sc->orig_pos);
    else
	fprintf(sfd, "Encoding: %d %d %d\n", enc, sc->unicodeenc,
		newgids!=NULL?newgids[sc->orig_pos]:sc->orig_pos);
    if ( sc->altuni ) {
	fprintf( sfd, "AltUni2:" );
	for ( altuni = sc->altuni; altuni!=NULL; altuni=altuni->next )
	    fprintf( sfd, " %06x.%06x.%x", altuni->unienc, altuni->vs, altuni->fid );
	putc( '\n', sfd);
    }
    if ( sc->glif_name ) {
        fprintf(sfd, "GlifName: ");
        if ( AllAscii(sc->glif_name))
	    fprintf(sfd, "%s", sc->glif_name );
        else {
	    SFDDumpUTF7Str(sfd,sc->glif_name);
        }
        putc('\n',sfd);
    }
    fprintf(sfd, "Width: %d\n", sc->width );
    if ( sc->vwidth!=sc->parent->ascent+sc->parent->descent )
	fprintf(sfd, "VWidth: %d\n", sc->vwidth );
    if ( sc->glyph_class!=0 )
	fprintf(sfd, "GlyphClass: %d\n", sc->glyph_class );
    if ( sc->unlink_rm_ovrlp_save_undo )
	fprintf(sfd, "UnlinkRmOvrlpSave: %d\n", sc->unlink_rm_ovrlp_save_undo );
    if ( sc->inspiro )
	fprintf(sfd, "InSpiro: %d\n", sc->inspiro );
    if ( sc->lig_caret_cnt_fixed )
	fprintf(sfd, "LigCaretCntFixed: %d\n", sc->lig_caret_cnt_fixed );
    if ( sc->changedsincelasthinted|| sc->manualhints || sc->widthset )
	fprintf(sfd, "Flags: %s%s%s%s%s\n",
		sc->changedsincelasthinted?"H":"",
		sc->manualhints?"M":"",
		sc->widthset?"W":"",
		sc->views!=NULL?"O":"",
		sc->instructions_out_of_date?"I":"");
    if ( sc->tex_height!=TEX_UNDEF || sc->tex_depth!=TEX_UNDEF )
	fprintf( sfd, "TeX: %d %d\n", sc->tex_height, sc->tex_depth );
    if ( sc->is_extended_shape || sc->italic_correction!=TEX_UNDEF ||
	    sc->top_accent_horiz!=TEX_UNDEF || sc->vert_variants!=NULL ||
	    sc->horiz_variants!=NULL || sc->mathkern!=NULL )
	SFDDumpCharMath(sfd,sc);
#if HANYANG
    if ( sc->compositionunit )
	fprintf( sfd, "CompositionUnit: %d %d\n", sc->jamo, sc->varient );
#endif
    SFDDumpHintList(sfd,"HStem: ", sc->hstem);
    SFDDumpHintList(sfd,"VStem: ", sc->vstem);
    SFDDumpDHintList(sfd,"DStem2: ", sc->dstem);
    if ( sc->countermask_cnt!=0 ) {
	fprintf( sfd, "CounterMasks: %d", sc->countermask_cnt );
	for ( i=0; i<sc->countermask_cnt; ++i ) {
	    putc(' ',sfd);
	    SFDDumpHintMask(sfd,&sc->countermasks[i]);
	}
	putc('\n',sfd);
    }
    if ( sc->ttf_instrs_len!=0 )
	SFDDumpTtfInstrs(sfd,sc);
    SFDDumpAnchorPoints(sfd,sc->anchor);
    fprintf( sfd, "LayerCount: %d\n", sc->layer_cnt );
    for ( i=0; i<sc->layer_cnt; ++i ) {
        if( saveUndoes && UndoRedoLimitToSave > 0) {
            if( sc->layers[i].undoes || sc->layers[i].redoes ) {
                fprintf(sfd, "UndoRedoHistory\n" );
                fprintf(sfd, "Layer: %d\n", i );
                Undoes *undo = 0;
                int idx   = 0;
                int limit = 0;

                fprintf(sfd, "Undoes\n" );
                idx = 0;
                undo = sc->layers[i].undoes;
                for( limit = UndoRedoLimitToSave;
                     undo && (limit==-1 || limit>0);
                     undo = undo->next, idx++ ) {
                    SFDDumpUndo( sfd, sc, undo, "Undo", idx );
                    if( limit > 0 )
                        limit--;
                }
                fprintf(sfd, "EndUndoes\n" );

		fprintf(sfd, "Redoes\n" );
                idx = 0;
                limit = UndoRedoLimitToSave;
                undo = sc->layers[i].redoes;
                for( limit = UndoRedoLimitToSave;
                     undo && (limit==-1 || limit>0);
                     undo = undo->next, idx++ ) {
                    SFDDumpUndo( sfd, sc, undo, "Redo", idx );
                    if( limit > 0 )
                        limit--;
                }
                fprintf(sfd, "EndRedoes\n" );
                fprintf(sfd, "EndUndoRedoHistory\n" );
            }
        }

	if ( sc->parent->multilayer ) {
	    fprintf(sfd, "Layer: %d  %d %d %d  #%06x %g  #%06x %g %g %s %s [%g %g %g %g] [",
		    i, sc->layers[i].dofill, sc->layers[i].dostroke, sc->layers[i].fillfirst,
		    sc->layers[i].fill_brush.col, (double) sc->layers[i].fill_brush.opacity,
		    sc->layers[i].stroke_pen.brush.col, (double) sc->layers[i].stroke_pen.brush.opacity,
		    (double) sc->layers[i].stroke_pen.width, joins[sc->layers[i].stroke_pen.linejoin], caps[sc->layers[i].stroke_pen.linecap],
		    (double) sc->layers[i].stroke_pen.trans[0], (double) sc->layers[i].stroke_pen.trans[1],
		    (double) sc->layers[i].stroke_pen.trans[2], (double) sc->layers[i].stroke_pen.trans[3] );
	    if ( sc->layers[i].stroke_pen.dashes[0]==0 && sc->layers[i].stroke_pen.dashes[1]==DASH_INHERITED )
		fprintf(sfd,"0 %d]\n", DASH_INHERITED);
	    else { int j;
		for ( j=0; j<DASH_MAX && sc->layers[i].stroke_pen.dashes[j]!=0; ++j )
		    fprintf( sfd,"%d ", sc->layers[i].stroke_pen.dashes[j]);
		fprintf(sfd,"]\n");
	    }
	    if ( sc->layers[i].fill_brush.gradient!=NULL )
		SFDDumpGradient(sfd,"FillGradient:", sc->layers[i].fill_brush.gradient );
	    else if ( sc->layers[i].fill_brush.pattern!=NULL )
		SFDDumpPattern(sfd,"FillPattern:", sc->layers[i].fill_brush.pattern );
	    if ( sc->layers[i].stroke_pen.brush.gradient!=NULL )
		SFDDumpGradient(sfd,"StrokeGradient:", sc->layers[i].stroke_pen.brush.gradient );
	    else if ( sc->layers[i].stroke_pen.brush.pattern!=NULL )
		SFDDumpPattern(sfd,"StrokePattern:", sc->layers[i].stroke_pen.brush.pattern );
	} else {
	    if ( sc->layers[i].images==NULL && sc->layers[i].splines==NULL &&
		    sc->layers[i].refs==NULL && (sc->layers[i].validation_state&vs_known) == 0 &&
		    sc->layers[i].python_persistent == NULL)
    continue;
	    if ( i==ly_back )
		fprintf( sfd, "Back\n" );
	    else if ( i==ly_fore )
		fprintf( sfd, "Fore\n" );
	    else
		fprintf(sfd, "Layer: %d\n", i );
	}
	for ( img=sc->layers[i].images; img!=NULL; img=img->next )
#ifndef _NO_LIBPNG
        if (WritePNGInSFD)
	    SFDDumpImagePNG(sfd,img);
        else
#endif
	    SFDDumpImage(sfd,img);
	if ( sc->layers[i].splines!=NULL ) {
	    fprintf(sfd, "SplineSet\n" );
	    SFDDumpSplineSet(sfd,sc->layers[i].splines,sc->layers[i].order2);
	}
	SFDDumpRefs(sfd,sc->layers[i].refs,newgids);
	SFDDumpGuidelines(sfd, sc->layers[i].guidelines);
	if ( sc->layers[i].validation_state&vs_known )
	    fprintf( sfd, "Validated: %d\n", sc->layers[i].validation_state );
        if ( sc->layers[i].python_persistent!=NULL )
	  SFDPickleMe(sfd,sc->layers[i].python_persistent,sc->layers[i].python_persistent_has_lists);
    }
    for ( v=0; v<2; ++v ) {
	kp = v ? sc->vkerns : sc->kerns;
	if ( kp!=NULL ) {
	    fprintf( sfd, v ? "VKerns2:" : "Kerns2:" );
	    for ( ; kp!=NULL; kp=kp->next ) {
            if ( !SFDOmit(kp->sc)) {
                fprintf( sfd, " %d %d ",
                         newgids!=NULL?newgids[kp->sc->orig_pos]:kp->sc->orig_pos,
                         kp->off );
                SFDDumpUTF7Str(sfd,kp->subtable->subtable_name);
                if ( kp->adjust!=NULL ) putc(' ',sfd);
                SFDDumpDeviceTable(sfd,kp->adjust);
            }
        }
	    fprintf(sfd, "\n" );
	}
    }
    for ( pst=sc->possub; pst!=NULL; pst=pst->next ) {
	if (( pst->subtable==NULL && pst->type!=pst_lcaret) || pst->type==pst_null )
	    /* Skip it */;
	else {
	    static const char *keywords[] = { "Null:", "Position2:", "PairPos2:",
		    "Substitution2:",
		    "AlternateSubs2:", "MultipleSubs2:", "Ligature2:",
		    "LCarets2:", NULL };
	    fprintf( sfd, "%s ", keywords[pst->type] );
	    if ( pst->subtable!=NULL ) {
		SFDDumpUTF7Str(sfd,pst->subtable->subtable_name);
		putc(' ',sfd);
	    }
	    if ( pst->type==pst_position ) {
		fprintf( sfd, "dx=%d dy=%d dh=%d dv=%d",
			pst->u.pos.xoff, pst->u.pos.yoff,
			pst->u.pos.h_adv_off, pst->u.pos.v_adv_off);
		SFDDumpValDevTab(sfd,pst->u.pos.adjust);
		putc('\n',sfd);
	    } else if ( pst->type==pst_pair ) {
		fprintf( sfd, "%s dx=%d dy=%d dh=%d dv=%d",
			pst->u.pair.paired,
			pst->u.pair.vr[0].xoff, pst->u.pair.vr[0].yoff,
			pst->u.pair.vr[0].h_adv_off, pst->u.pair.vr[0].v_adv_off );
		SFDDumpValDevTab(sfd,pst->u.pair.vr[0].adjust);
		fprintf( sfd, " dx=%d dy=%d dh=%d dv=%d",
			pst->u.pair.vr[1].xoff, pst->u.pair.vr[1].yoff,
			pst->u.pair.vr[1].h_adv_off, pst->u.pair.vr[1].v_adv_off);
		SFDDumpValDevTab(sfd,pst->u.pair.vr[1].adjust);
		putc('\n',sfd);
	    } else if ( pst->type==pst_lcaret ) {
		int i;
		fprintf( sfd, "%d ", pst->u.lcaret.cnt );
		for ( i=0; i<pst->u.lcaret.cnt; ++i ) {
		    fprintf( sfd, "%d", pst->u.lcaret.carets[i] );
                    if ( i<pst->u.lcaret.cnt-1 ) putc(' ',sfd);
                }
		fprintf( sfd, "\n" );
	    } else
		fprintf( sfd, "%s\n", pst->u.lig.components );
	}
    }
    if ( sc->comment!=NULL ) {
	fprintf( sfd, "Comment: " );
	SFDDumpUTF7Str(sfd,sc->comment);
	putc('\n',sfd);
    }
    if ( sc->user_decomp != NULL ) {
	fprintf( sfd, "Decomposition: " );
	char* temp_ud = u2utf8_copy(sc->user_decomp);
	SFDDumpUTF7Str(sfd, temp_ud);
	free(temp_ud);
	putc('\n',sfd);
    }
    if ( sc->color!=COLOR_DEFAULT )
	fprintf( sfd, "Colour: %x\n", (int) sc->color );
    if ( sc->parent->multilayer ) {
	if ( sc->tile_margin!=0 )
	    fprintf( sfd, "TileMargin: %g\n", (double) sc->tile_margin );
	else if ( sc->tile_bounds.minx!=0 || sc->tile_bounds.maxx!=0 )
	    fprintf( sfd, "TileBounds: %g %g %g %g\n", (double) sc->tile_bounds.minx, (double) sc->tile_bounds.miny, (double) sc->tile_bounds.maxx, (double) sc->tile_bounds.maxy );
    }
    fprintf(sfd,"EndChar\n" );
}