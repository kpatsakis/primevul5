void bcf_hrec_debug(FILE *fp, bcf_hrec_t *hrec)
{
    fprintf(fp, "key=[%s] value=[%s]", hrec->key, hrec->value?hrec->value:"");
    int i;
    for (i=0; i<hrec->nkeys; i++)
        fprintf(fp, "\t[%s]=[%s]", hrec->keys[i],hrec->vals[i]);
    fprintf(fp, "\n");
}