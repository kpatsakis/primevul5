image_as_dict(Image *img) {
#define U(x) #x, img->x
    return Py_BuildValue("{sI sI sI sI sK sI sO sO sN}",
        U(texture_id), U(client_id), U(width), U(height), U(internal_id), U(refcnt),
        "data_loaded", img->data_loaded ? Py_True : Py_False,
        "is_4byte_aligned", img->load_data.is_4byte_aligned ? Py_True : Py_False,
        "data", Py_BuildValue("y#", img->load_data.data, img->load_data.data_sz)
    );
#undef U

}