W(set_send_to_gpu) {
    send_to_gpu = PyObject_IsTrue(args) ? true : false;
    Py_RETURN_NONE;
}