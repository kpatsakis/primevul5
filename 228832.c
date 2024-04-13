int SFD_DumpSplineFontMetadata( FILE *sfd, SplineFont *sf )
{
    int i, j;
    struct ttflangname *ln;
    struct ttf_table *tab;
    KernClass *kc;
    FPST *fpst;
    ASM *sm;
    int isv;
    int err = false;
    int isgpos;
    OTLookup *otl;
    struct lookup_subtable *sub;
    FeatureScriptLangList *fl;
    struct scriptlanglist *sl;

    fprintf(sfd, "FontName: %s\n", sf->fontname );
    if ( sf->fullname!=NULL )
	fprintf(sfd, "FullName: %s\n", sf->fullname );
    if ( sf->familyname!=NULL )
	fprintf(sfd, "FamilyName: %s\n", sf->familyname );
    if ( sf->weight!=NULL )
	fprintf(sfd, "Weight: %s\n", sf->weight );
    if ( sf->copyright!=NULL )
	putstring(sfd, "Copyright: ", sf->copyright );
    if ( sf->comments!=NULL ) {
	fprintf( sfd, "UComments: " );
	SFDDumpUTF7Str(sfd,sf->comments);
	putc('\n',sfd);
    }
    if ( sf->fontlog!=NULL ) {
	fprintf( sfd, "FontLog: " );
	SFDDumpUTF7Str(sfd,sf->fontlog);
	putc('\n',sfd);
    }

    if ( sf->version!=NULL )
	fprintf(sfd, "Version: %s\n", sf->version );
    if ( sf->styleMapFamilyName!=NULL )
	fprintf(sfd, "StyleMapFamilyName: %s\n", sf->styleMapFamilyName );
    if ( sf->fondname!=NULL )
	fprintf(sfd, "FONDName: %s\n", sf->fondname );
    if ( sf->defbasefilename!=NULL )
	fprintf(sfd, "DefaultBaseFilename: %s\n", sf->defbasefilename );
    if ( sf->strokewidth!=0 )
	fprintf(sfd, "StrokeWidth: %g\n", (double) sf->strokewidth );
    fprintf(sfd, "ItalicAngle: %g\n", (double) sf->italicangle );
    fprintf(sfd, "UnderlinePosition: %g\n", (double) sf->upos );
    fprintf(sfd, "UnderlineWidth: %g\n", (double) sf->uwidth );
    fprintf(sfd, "Ascent: %d\n", sf->ascent );
    fprintf(sfd, "Descent: %d\n", sf->descent );
    fprintf(sfd, "InvalidEm: %d\n", sf->invalidem );
    if ( sf->sfntRevision!=sfntRevisionUnset )
	fprintf(sfd, "sfntRevision: 0x%08x\n", sf->sfntRevision );
    if ( sf->woffMajor!=woffUnset ) {
	fprintf(sfd, "woffMajor: %d\n", sf->woffMajor );
	fprintf(sfd, "woffMinor: %d\n", sf->woffMinor );
    }
    if ( sf->woffMetadata!=NULL ) {
	fprintf( sfd, "woffMetadata: " );
	SFDDumpUTF7Str(sfd,sf->woffMetadata);
	putc('\n',sfd);
    }
    if ( sf->ufo_ascent!=0 )
	fprintf(sfd, "UFOAscent: %g\n", (double) sf->ufo_ascent );
    if ( sf->ufo_descent!=0 )
	fprintf(sfd, "UFODescent: %g\n", (double) sf->ufo_descent );
    fprintf(sfd, "LayerCount: %d\n", sf->layer_cnt );
    for ( i=0; i<sf->layer_cnt; ++i ) {
	fprintf( sfd, "Layer: %d %d ", i, sf->layers[i].order2/*, sf->layers[i].background*/ );
	SFDDumpUTF7Str(sfd,sf->layers[i].name);
	fprintf( sfd, " %d", sf->layers[i].background );
	if (sf->layers[i].ufo_path != NULL) { putc(' ',sfd); SFDDumpUTF7Str(sfd,sf->layers[i].ufo_path); }
	putc('\n',sfd);
    }
    // TODO: Output U. F. O. layer path.
    if (sf->preferred_kerning != 0) fprintf(sfd, "PreferredKerning: %d\n", sf->preferred_kerning);
    if ( sf->strokedfont )
	fprintf(sfd, "StrokedFont: %d\n", sf->strokedfont );
    else if ( sf->multilayer )
	fprintf(sfd, "MultiLayer: %d\n", sf->multilayer );
    if ( sf->hasvmetrics )
	fprintf(sfd, "HasVMetrics: %d\n", sf->hasvmetrics );
    if ( sf->use_xuid && sf->changed_since_xuidchanged )
	fprintf(sfd, "NeedsXUIDChange: 1\n" );
    if ( sf->xuid!=NULL )
	fprintf(sfd, "XUID: %s\n", sf->xuid );
    if ( sf->uniqueid!=0 )
	fprintf(sfd, "UniqueID: %d\n", sf->uniqueid );
    if ( sf->use_xuid )
	fprintf(sfd, "UseXUID: 1\n" );
    if ( sf->use_uniqueid )
	fprintf(sfd, "UseUniqueID: 1\n" );
    if ( sf->horiz_base!=NULL )
	SFDDumpBase(sfd,"BaseHoriz:",sf->horiz_base);
    if ( sf->vert_base!=NULL )
	SFDDumpBase(sfd,"BaseVert:",sf->vert_base);
    if ( sf->pfminfo.stylemap!=-1 )
	fprintf(sfd, "StyleMap: 0x%04x\n", sf->pfminfo.stylemap );
    if ( sf->pfminfo.fstype!=-1 )
	fprintf(sfd, "FSType: %d\n", sf->pfminfo.fstype );
    fprintf(sfd, "OS2Version: %d\n", sf->os2_version );
    fprintf(sfd, "OS2_WeightWidthSlopeOnly: %d\n", sf->weight_width_slope_only );
    fprintf(sfd, "OS2_UseTypoMetrics: %d\n", sf->use_typo_metrics );
    fprintf(sfd, "CreationTime: %lld\n", sf->creationtime );
    fprintf(sfd, "ModificationTime: %lld\n", sf->modificationtime );
    if ( sf->pfminfo.pfmset ) {
	fprintf(sfd, "PfmFamily: %d\n", sf->pfminfo.pfmfamily );
	fprintf(sfd, "TTFWeight: %d\n", sf->pfminfo.weight );
	fprintf(sfd, "TTFWidth: %d\n", sf->pfminfo.width );
	fprintf(sfd, "LineGap: %d\n", sf->pfminfo.linegap );
	fprintf(sfd, "VLineGap: %d\n", sf->pfminfo.vlinegap );
	/*putc('\n',sfd);*/
    }
    if ( sf->pfminfo.panose_set ) {
	fprintf(sfd, "Panose:" );
	for ( i=0; i<10; ++i )
	    fprintf( sfd, " %d", sf->pfminfo.panose[i]);
	putc('\n',sfd);
    }
    fprintf(sfd, "OS2TypoAscent: %d\n", sf->pfminfo.os2_typoascent );
    fprintf(sfd, "OS2TypoAOffset: %d\n", sf->pfminfo.typoascent_add );
    fprintf(sfd, "OS2TypoDescent: %d\n", sf->pfminfo.os2_typodescent );
    fprintf(sfd, "OS2TypoDOffset: %d\n", sf->pfminfo.typodescent_add );
    fprintf(sfd, "OS2TypoLinegap: %d\n", sf->pfminfo.os2_typolinegap );
    fprintf(sfd, "OS2WinAscent: %d\n", sf->pfminfo.os2_winascent );
    fprintf(sfd, "OS2WinAOffset: %d\n", sf->pfminfo.winascent_add );
    fprintf(sfd, "OS2WinDescent: %d\n", sf->pfminfo.os2_windescent );
    fprintf(sfd, "OS2WinDOffset: %d\n", sf->pfminfo.windescent_add );
    fprintf(sfd, "HheadAscent: %d\n", sf->pfminfo.hhead_ascent );
    fprintf(sfd, "HheadAOffset: %d\n", sf->pfminfo.hheadascent_add );
    fprintf(sfd, "HheadDescent: %d\n", sf->pfminfo.hhead_descent );
    fprintf(sfd, "HheadDOffset: %d\n", sf->pfminfo.hheaddescent_add );
    if ( sf->pfminfo.subsuper_set ) {
	fprintf(sfd, "OS2SubXSize: %d\n", sf->pfminfo.os2_subxsize );
	fprintf(sfd, "OS2SubYSize: %d\n", sf->pfminfo.os2_subysize );
	fprintf(sfd, "OS2SubXOff: %d\n", sf->pfminfo.os2_subxoff );
	fprintf(sfd, "OS2SubYOff: %d\n", sf->pfminfo.os2_subyoff );
	fprintf(sfd, "OS2SupXSize: %d\n", sf->pfminfo.os2_supxsize );
	fprintf(sfd, "OS2SupYSize: %d\n", sf->pfminfo.os2_supysize );
	fprintf(sfd, "OS2SupXOff: %d\n", sf->pfminfo.os2_supxoff );
	fprintf(sfd, "OS2SupYOff: %d\n", sf->pfminfo.os2_supyoff );
	fprintf(sfd, "OS2StrikeYSize: %d\n", sf->pfminfo.os2_strikeysize );
	fprintf(sfd, "OS2StrikeYPos: %d\n", sf->pfminfo.os2_strikeypos );
    }
    if ( sf->pfminfo.os2_capheight!=0 )
    fprintf(sfd, "OS2CapHeight: %d\n", sf->pfminfo.os2_capheight );
    if ( sf->pfminfo.os2_xheight!=0 )
    fprintf(sfd, "OS2XHeight: %d\n", sf->pfminfo.os2_xheight );
    if ( sf->pfminfo.os2_family_class!=0 )
	fprintf(sfd, "OS2FamilyClass: %d\n", sf->pfminfo.os2_family_class );
    if ( sf->pfminfo.os2_vendor[0]!='\0' ) {
	fprintf(sfd, "OS2Vendor: '%c%c%c%c'\n",
		sf->pfminfo.os2_vendor[0], sf->pfminfo.os2_vendor[1],
		sf->pfminfo.os2_vendor[2], sf->pfminfo.os2_vendor[3] );
    }
    if ( sf->pfminfo.hascodepages )
	fprintf(sfd, "OS2CodePages: %08x.%08x\n", sf->pfminfo.codepages[0], sf->pfminfo.codepages[1]);
    if ( sf->pfminfo.hasunicoderanges )
	fprintf(sfd, "OS2UnicodeRanges: %08x.%08x.%08x.%08x\n",
		sf->pfminfo.unicoderanges[0], sf->pfminfo.unicoderanges[1],
		sf->pfminfo.unicoderanges[2], sf->pfminfo.unicoderanges[3] );
    if ( sf->macstyle!=-1 )
	fprintf(sfd, "MacStyle: %d\n", sf->macstyle );
    /* Must come before any kerning classes, anchor classes, conditional psts */
    /* state machines, psts, kerning pairs, etc. */
    for ( isgpos=0; isgpos<2; ++isgpos ) {
	for ( otl = isgpos ? sf->gpos_lookups : sf->gsub_lookups; otl!=NULL; otl = otl->next ) {
	    fprintf( sfd, "Lookup: %d %d %d ", otl->lookup_type, otl->lookup_flags, otl->store_in_afm );
	    SFDDumpUTF7Str(sfd,otl->lookup_name);
	    fprintf( sfd, " { " );
	    for ( sub=otl->subtables; sub!=NULL; sub=sub->next ) {
		SFDDumpUTF7Str(sfd,sub->subtable_name);
		putc(' ',sfd);
		if ( otl->lookup_type==gsub_single && sub->suffix!=NULL ) {
		    putc('(',sfd);
		    SFDDumpUTF7Str(sfd,sub->suffix);
		    putc(')',sfd);
		} else if ( otl->lookup_type==gpos_pair && sub->vertical_kerning )
		    fprintf(sfd,"(1)");
		if ( otl->lookup_type==gpos_pair && (sub->separation!=0 || sub->kerning_by_touch))
		    fprintf(sfd,"[%d,%d,%d]", sub->separation, sub->minkern, sub->kerning_by_touch+2*sub->onlyCloser+4*sub->dontautokern );
		putc(' ',sfd);
	    }
	    fprintf( sfd, "} [" );
	    for ( fl=otl->features; fl!=NULL; fl=fl->next ) {
		if ( fl->ismac )
		    fprintf( sfd, "<%d,%d> (",
			    (int) (fl->featuretag>>16),
			    (int) (fl->featuretag&0xffff));
		else
		    fprintf( sfd, "'%c%c%c%c' (",
			    (int) (fl->featuretag>>24), (int) ((fl->featuretag>>16)&0xff),
			    (int) ((fl->featuretag>>8)&0xff), (int) (fl->featuretag&0xff) );
		for ( sl= fl->scripts; sl!=NULL; sl = sl->next ) {
		    fprintf( sfd, "'%c%c%c%c' <",
			    (int) (sl->script>>24), (int) ((sl->script>>16)&0xff),
			    (int) ((sl->script>>8)&0xff), (int) (sl->script&0xff) );
		    for ( i=0; i<sl->lang_cnt; ++i ) {
			uint32 lang = i<MAX_LANG ? sl->langs[i] : sl->morelangs[i-MAX_LANG];
			fprintf( sfd, "'%c%c%c%c' ",
				(int) (lang>>24), (int) ((lang>>16)&0xff),
				(int) ((lang>>8)&0xff), (int) (lang&0xff) );
		    }
		    fprintf( sfd, "> " );
		}
		fprintf( sfd, ") " );
	    }
	    fprintf( sfd, "]\n" );
	}
    }

    if ( sf->mark_class_cnt!=0 ) {
	fprintf( sfd, "MarkAttachClasses: %d\n", sf->mark_class_cnt );
	for ( i=1; i<sf->mark_class_cnt; ++i ) {	/* Class 0 is unused */
	    SFDDumpUTF7Str(sfd, sf->mark_class_names[i]);
	    putc(' ',sfd);
	    if ( sf->mark_classes[i]!=NULL )
		fprintf( sfd, "%d %s\n", (int) strlen(sf->mark_classes[i]),
			sf->mark_classes[i] );
	    else
		fprintf( sfd, "0 \n" );
	}
    }
    if ( sf->mark_set_cnt!=0 ) {
	fprintf( sfd, "MarkAttachSets: %d\n", sf->mark_set_cnt );
	for ( i=0; i<sf->mark_set_cnt; ++i ) {	/* Set 0 is used */
	    SFDDumpUTF7Str(sfd, sf->mark_set_names[i]);
	    putc(' ',sfd);
	    if ( sf->mark_sets[i]!=NULL )
		fprintf( sfd, "%d %s\n", (int) strlen(sf->mark_sets[i]),
			sf->mark_sets[i] );
	    else
		fprintf( sfd, "0 \n" );
	}
    }

    fprintf( sfd, "DEI: 91125\n" );
    for ( isv=0; isv<2; ++isv ) {
	for ( kc=isv ? sf->vkerns : sf->kerns; kc!=NULL; kc = kc->next ) {
	  if (kc->firsts_names == NULL && kc->seconds_names == NULL && kc->firsts_flags == NULL && kc->seconds_flags == NULL) {
	    fprintf( sfd, "%s: %d%s %d ", isv ? "VKernClass2" : "KernClass2",
		    kc->first_cnt, kc->firsts[0]!=NULL?"+":"",
		    kc->second_cnt );
	    SFDDumpUTF7Str(sfd,kc->subtable->subtable_name);
	    putc('\n',sfd);
	    if ( kc->firsts[0]!=NULL )
	      fprintf( sfd, " %d %s\n", (int)strlen(kc->firsts[0]),
		       kc->firsts[0]);
	    for ( i=1; i<kc->first_cnt; ++i )
	      fprintf( sfd, " %d %s\n", (int)strlen(kc->firsts[i]),
		       kc->firsts[i]);
	    for ( i=1; i<kc->second_cnt; ++i )
	      fprintf( sfd, " %d %s\n", (int)strlen(kc->seconds[i]),
		       kc->seconds[i]);
	    for ( i=0; i<kc->first_cnt*kc->second_cnt; ++i ) {
		fprintf( sfd, " %d", kc->offsets[i]);
		putc(' ',sfd);
		SFDDumpDeviceTable(sfd,&kc->adjusts[i]);
	    }
	    fprintf( sfd, "\n" );
	  } else {
	    fprintf( sfd, "%s: %d%s %d ", isv ? "VKernClass3" : "KernClass3",
		    kc->first_cnt, kc->firsts[0]!=NULL?"+":"",
		    kc->second_cnt );
	    SFDDumpUTF7Str(sfd,kc->subtable->subtable_name);
	    putc('\n',sfd);
	    if ( kc->firsts[0]!=NULL ) {
	      fprintf( sfd, " %d ", ((kc->firsts_flags && kc->firsts_flags[0]) ? kc->firsts_flags[0] : 0));
	      SFDDumpUTF7Str(sfd, ((kc->firsts_names && kc->firsts_names[0]) ? kc->firsts_names[0] : ""));
	      fprintf( sfd, " " );
	      SFDDumpUTF7Str(sfd,kc->firsts[0]);
	      fprintf( sfd, "\n" );
	    }
	    for ( i=1; i<kc->first_cnt; ++i ) {
	      fprintf( sfd, " %d ", ((kc->firsts_flags && kc->firsts_flags[i]) ? kc->firsts_flags[i] : 0));
	      SFDDumpUTF7Str(sfd, ((kc->firsts_names && kc->firsts_names[i]) ? kc->firsts_names[i] : ""));
	      fprintf( sfd, " " );
	      SFDDumpUTF7Str(sfd,kc->firsts[i]);
	      fprintf( sfd, "\n" );
	    }
	    for ( i=1; i<kc->second_cnt; ++i ) {
	      fprintf( sfd, " %d ", ((kc->seconds_flags && kc->seconds_flags[i]) ? kc->seconds_flags[i] : 0));
	      SFDDumpUTF7Str(sfd, ((kc->seconds_names && kc->seconds_names[i]) ? kc->seconds_names[i] : ""));
	      fprintf( sfd, " " );
	      SFDDumpUTF7Str(sfd,kc->seconds[i]);
	      fprintf( sfd, "\n" );
	    }
	    for ( i=0; i<kc->first_cnt*kc->second_cnt; ++i ) {
		fprintf( sfd, " %d %d", ((kc->offsets_flags && kc->offsets_flags[i]) ? kc->offsets_flags[i] : 0), kc->offsets[i]);
		putc(' ',sfd);
		SFDDumpDeviceTable(sfd,&kc->adjusts[i]);
	    }
	    fprintf( sfd, "\n" );
	  }
	}
    }
    for ( fpst=sf->possub; fpst!=NULL; fpst=fpst->next ) {
	static const char *keywords[] = { "ContextPos2:", "ContextSub2:", "ChainPos2:", "ChainSub2:", "ReverseChain2:", NULL };
	static const char *formatkeys[] = { "glyph", "class", "coverage", "revcov", NULL };
	fprintf( sfd, "%s %s ", keywords[fpst->type-pst_contextpos],
		formatkeys[fpst->format] );
	SFDDumpUTF7Str(sfd,fpst->subtable->subtable_name);
	fprintf( sfd, " %d %d %d %d\n",
		fpst->nccnt, fpst->bccnt, fpst->fccnt, fpst->rule_cnt );
	if ( fpst->nccnt>0 && fpst->nclass[0]!=NULL )
	  fprintf( sfd, "  Class0: %d %s\n", (int)strlen(fpst->nclass[0]),
		   fpst->nclass[0]);
	for ( i=1; i<fpst->nccnt; ++i )
	  fprintf( sfd, "  Class: %d %s\n", (int)strlen(fpst->nclass[i]),
		   fpst->nclass[i]);
	for ( i=1; i<fpst->bccnt; ++i )
	  fprintf( sfd, "  BClass: %d %s\n", (int)strlen(fpst->bclass[i]),
		   fpst->bclass[i]);
	for ( i=1; i<fpst->fccnt; ++i )
	  fprintf( sfd, "  FClass: %d %s\n", (int)strlen(fpst->fclass[i]),
		   fpst->fclass[i]);
	for ( i=0; i<fpst->rule_cnt; ++i ) {
	    switch ( fpst->format ) {
	      case pst_glyphs:
		fprintf( sfd, " String: %d %s\n",
			 (int)strlen(fpst->rules[i].u.glyph.names),
			 fpst->rules[i].u.glyph.names);
		if ( fpst->rules[i].u.glyph.back!=NULL )
		  fprintf( sfd, " BString: %d %s\n",
			   (int)strlen(fpst->rules[i].u.glyph.back),
			   fpst->rules[i].u.glyph.back);
		else
		    fprintf( sfd, " BString: 0\n");
		if ( fpst->rules[i].u.glyph.fore!=NULL )
		  fprintf( sfd, " FString: %d %s\n",
			   (int)strlen(fpst->rules[i].u.glyph.fore),
			   fpst->rules[i].u.glyph.fore);
		else
		    fprintf( sfd, " FString: 0\n");
	      break;
	      case pst_class:
		fprintf( sfd, " %d %d %d\n  ClsList:", fpst->rules[i].u.class.ncnt, fpst->rules[i].u.class.bcnt, fpst->rules[i].u.class.fcnt );
		for ( j=0; j<fpst->rules[i].u.class.ncnt; ++j )
		    fprintf( sfd, " %d", fpst->rules[i].u.class.nclasses[j]);
		fprintf( sfd, "\n  BClsList:" );
		for ( j=0; j<fpst->rules[i].u.class.bcnt; ++j )
		    fprintf( sfd, " %d", fpst->rules[i].u.class.bclasses[j]);
		fprintf( sfd, "\n  FClsList:" );
		for ( j=0; j<fpst->rules[i].u.class.fcnt; ++j )
		    fprintf( sfd, " %d", fpst->rules[i].u.class.fclasses[j]);
		fprintf( sfd, "\n" );
	      break;
	      case pst_coverage:
	      case pst_reversecoverage:
		fprintf( sfd, " %d %d %d\n", fpst->rules[i].u.coverage.ncnt, fpst->rules[i].u.coverage.bcnt, fpst->rules[i].u.coverage.fcnt );
		for ( j=0; j<fpst->rules[i].u.coverage.ncnt; ++j )
		  fprintf( sfd, "  Coverage: %d %s\n",
			   (int)strlen(fpst->rules[i].u.coverage.ncovers[j]),
			   fpst->rules[i].u.coverage.ncovers[j]);
		for ( j=0; j<fpst->rules[i].u.coverage.bcnt; ++j )
		  fprintf( sfd, "  BCoverage: %d %s\n",
			   (int)strlen(fpst->rules[i].u.coverage.bcovers[j]),
			   fpst->rules[i].u.coverage.bcovers[j]);
		for ( j=0; j<fpst->rules[i].u.coverage.fcnt; ++j )
		  fprintf( sfd, "  FCoverage: %d %s\n",
			   (int)strlen(fpst->rules[i].u.coverage.fcovers[j]),
			   fpst->rules[i].u.coverage.fcovers[j]);
	      break;
	      default:
	      break;
	    }
	    switch ( fpst->format ) {
	      case pst_glyphs:
	      case pst_class:
	      case pst_coverage:
		fprintf( sfd, " %d\n", fpst->rules[i].lookup_cnt );
		for ( j=0; j<fpst->rules[i].lookup_cnt; ++j ) {
		    fprintf( sfd, "  SeqLookup: %d ",
			    fpst->rules[i].lookups[j].seq );
		    SFDDumpUTF7Str(sfd,fpst->rules[i].lookups[j].lookup->lookup_name);
		    putc('\n',sfd);
		}
	      break;
	      case pst_reversecoverage:
		fprintf( sfd, "  Replace: %d %s\n",
			 (int)strlen(fpst->rules[i].u.rcoverage.replacements),
			 fpst->rules[i].u.rcoverage.replacements);
	      break;
	      default:
	      break;
	    }
	}
	/* It would make more sense to output these up near the classes */
	/*  but that would break backwards compatibility (old parsers will */
	/*  ignore these entries if they are at the end, new parsers will */
	/*  read them */
	SFDFpstClassNamesOut(sfd,fpst->nccnt,fpst->nclassnames,"ClassNames");
	SFDFpstClassNamesOut(sfd,fpst->bccnt,fpst->bclassnames,"BClassNames");
	SFDFpstClassNamesOut(sfd,fpst->fccnt,fpst->fclassnames,"FClassNames");
	fprintf( sfd, "EndFPST\n" );
    }
    struct ff_glyphclasses *grouptmp;
    for ( grouptmp = sf->groups; grouptmp != NULL; grouptmp = grouptmp->next ) {
      fprintf(sfd, "Group: ");
      SFDDumpUTF7Str(sfd, grouptmp->classname); fprintf(sfd, " ");
      SFDDumpUTF7Str(sfd, grouptmp->glyphs); fprintf(sfd, "\n");
    }
    struct ff_rawoffsets *groupkerntmp;
    for ( groupkerntmp = sf->groupkerns; groupkerntmp != NULL; groupkerntmp = groupkerntmp->next ) {
      fprintf(sfd, "GroupKern: ");
      SFDDumpUTF7Str(sfd, groupkerntmp->left); fprintf(sfd, " ");
      SFDDumpUTF7Str(sfd, groupkerntmp->right); fprintf(sfd, " ");
      fprintf(sfd, "%d\n", groupkerntmp->offset);
    }
    for ( groupkerntmp = sf->groupvkerns; groupkerntmp != NULL; groupkerntmp = groupkerntmp->next ) {
      fprintf(sfd, "GroupVKern: ");
      SFDDumpUTF7Str(sfd, groupkerntmp->left); fprintf(sfd, " ");
      SFDDumpUTF7Str(sfd, groupkerntmp->right); fprintf(sfd, " ");
      fprintf(sfd, "%d\n", groupkerntmp->offset);
    }
    for ( sm=sf->sm; sm!=NULL; sm=sm->next ) {
	static const char *keywords[] = { "MacIndic2:", "MacContext2:", "MacLigature2:", "unused", "MacSimple2:", "MacInsert2:",
	    "unused", "unused", "unused", "unused", "unused", "unused",
	    "unused", "unused", "unused", "unused", "unused", "MacKern2:",
	    NULL };
	fprintf( sfd, "%s ", keywords[sm->type-asm_indic] );
	SFDDumpUTF7Str(sfd,sm->subtable->subtable_name);
	fprintf( sfd, " %d %d %d\n", sm->flags, sm->class_cnt, sm->state_cnt );
	for ( i=4; i<sm->class_cnt; ++i )
	  fprintf( sfd, "  Class: %d %s\n", (int)strlen(sm->classes[i]),
		   sm->classes[i]);
	for ( i=0; i<sm->class_cnt*sm->state_cnt; ++i ) {
	    fprintf( sfd, " %d %d ", sm->state[i].next_state, sm->state[i].flags );
	    if ( sm->type==asm_context ) {
		if ( sm->state[i].u.context.mark_lookup==NULL )
		    putc('~',sfd);
		else
		    SFDDumpUTF7Str(sfd,sm->state[i].u.context.mark_lookup->lookup_name);
		putc(' ',sfd);
		if ( sm->state[i].u.context.cur_lookup==0 )
		    putc('~',sfd);
		else
		    SFDDumpUTF7Str(sfd,sm->state[i].u.context.cur_lookup->lookup_name);
		putc(' ',sfd);
	    } else if ( sm->type == asm_insert ) {
		if ( sm->state[i].u.insert.mark_ins==NULL )
		    fprintf( sfd, "0 ");
		else
		  fprintf( sfd, "%d %s ",
			   (int)strlen(sm->state[i].u.insert.mark_ins),
			   sm->state[i].u.insert.mark_ins );
		if ( sm->state[i].u.insert.cur_ins==NULL )
		    fprintf( sfd, "0 ");
		else
		  fprintf( sfd, "%d %s ",
			   (int)strlen(sm->state[i].u.insert.cur_ins),
			   sm->state[i].u.insert.cur_ins );
	    } else if ( sm->type == asm_kern ) {
		fprintf( sfd, "%d ", sm->state[i].u.kern.kcnt );
		for ( j=0; j<sm->state[i].u.kern.kcnt; ++j )
		    fprintf( sfd, "%d ", sm->state[i].u.kern.kerns[j]);
	    }
	    putc('\n',sfd);
	}
	fprintf( sfd, "EndASM\n" );
    }
    SFDDumpMacFeat(sfd,sf->features);
    SFDDumpJustify(sfd,sf);
    for ( tab = sf->ttf_tables; tab!=NULL ; tab = tab->next )
	SFDDumpTtfTable(sfd,tab,sf);
    for ( tab = sf->ttf_tab_saved; tab!=NULL ; tab = tab->next )
	SFDDumpTtfTable(sfd,tab,sf);
    for ( ln = sf->names; ln!=NULL; ln=ln->next )
	SFDDumpLangName(sfd,ln);
    if ( sf->gasp_cnt!=0 )
	SFDDumpGasp(sfd,sf);
    if ( sf->design_size!=0 )
	SFDDumpDesignSize(sfd,sf);
    if ( sf->feat_names!=NULL )
	SFDDumpOtfFeatNames(sfd,sf);

return( err );
}