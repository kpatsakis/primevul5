static void nfs4_free_createdata(struct nfs4_createdata *data)
{
	nfs4_label_free(data->label);
	kfree(data);
}