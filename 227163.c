static void bcf_set_variant_type(const char *ref, const char *alt, variant_t *var)
{
    if ( *alt == '*' && !alt[1] ) { var->n = 0; var->type = VCF_OVERLAP; return; }  // overlapping variant

    // The most frequent case
    if ( !ref[1] && !alt[1] )
    {
        if ( *alt == '.' || *ref==*alt ) { var->n = 0; var->type = VCF_REF; return; }
        if ( *alt == 'X' ) { var->n = 0; var->type = VCF_REF; return; }  // mpileup's X allele shouldn't be treated as variant
        var->n = 1; var->type = VCF_SNP; return;
    }
    if ( alt[0]=='<' )
    {
        if ( alt[1]=='X' && alt[2]=='>' ) { var->n = 0; var->type = VCF_REF; return; }  // mpileup's X allele shouldn't be treated as variant
        if ( alt[1]=='*' && alt[2]=='>' ) { var->n = 0; var->type = VCF_REF; return; }
        if ( !strcmp("NON_REF>",alt+1) ) { var->n = 0; var->type = VCF_REF; return; }
        var->type = VCF_OTHER;
        return;
    }

    const char *r = ref, *a = alt;
    while (*r && *a && toupper_c(*r)==toupper_c(*a) ) { r++; a++; }     // unfortunately, matching REF,ALT case is not guaranteed

    if ( *a && !*r )
    {
        if ( *a==']' || *a=='[' ) { var->type = VCF_BND; return; }
        while ( *a ) a++;
        var->n = (a-alt)-(r-ref); var->type = VCF_INDEL; return;
    }
    else if ( *r && !*a )
    {
        while ( *r ) r++;
        var->n = (a-alt)-(r-ref); var->type = VCF_INDEL; return;
    }
    else if ( !*r && !*a )
    {
        var->n = 0; var->type = VCF_REF; return;
    }

    const char *re = r, *ae = a;
    while ( re[1] ) re++;
    while ( ae[1] ) ae++;
    while ( re>r && ae>a && toupper_c(*re)==toupper_c(*ae) ) { re--; ae--; }
    if ( ae==a )
    {
        if ( re==r ) { var->n = 1; var->type = VCF_SNP; return; }
        var->n = -(re-r);
        if ( toupper_c(*re)==toupper_c(*ae) ) { var->type = VCF_INDEL; return; }
        var->type = VCF_OTHER; return;
    }
    else if ( re==r )
    {
        var->n = ae-a;
        if ( toupper_c(*re)==toupper_c(*ae) ) { var->type = VCF_INDEL; return; }
        var->type = VCF_OTHER; return;
    }

    var->type = ( re-r == ae-a ) ? VCF_MNP : VCF_OTHER;
    var->n = ( re-r > ae-a ) ? -(re-r+1) : ae-a+1;

    // should do also complex events, SVs, etc...
}