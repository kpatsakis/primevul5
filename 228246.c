trim_predicate(Image *img) {
    return !img->data_loaded || !img->refcnt;
}