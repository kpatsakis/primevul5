static int load_dh_params(SSL_CTX *ctx, char *file)
{
    DH *ret = 0;
    BIO *bio;

    if ((bio = BIO_new_file(file, "r")) == NULL) {
        spice_warning("Could not open DH file");
        return -1;
    }

    ret = PEM_read_bio_DHparams(bio, NULL, NULL, NULL);
    BIO_free(bio);
    if (ret == 0) {
        spice_warning("Could not read DH params");
        return -1;
    }


    if (SSL_CTX_set_tmp_dh(ctx, ret) < 0) {
        spice_warning("Could not set DH params");
        return -1;
    }

    return 0;
}
