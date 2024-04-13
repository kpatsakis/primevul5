int bcf_is_snp(bcf1_t *v)
{
    int i;
    bcf_unpack(v, BCF_UN_STR);
    for (i = 0; i < v->n_allele; ++i)
    {
        if ( v->d.allele[i][1]==0 && v->d.allele[i][0]!='*' ) continue;

        // mpileup's <X> allele, see also below. This is not completely satisfactory,
        // a general library is here narrowly tailored to fit samtools.
        if ( v->d.allele[i][0]=='<' && v->d.allele[i][1]=='X' && v->d.allele[i][2]=='>' ) continue;
        if ( v->d.allele[i][0]=='<' && v->d.allele[i][1]=='*' && v->d.allele[i][2]=='>' ) continue;

        break;
    }
    return i == v->n_allele;
}