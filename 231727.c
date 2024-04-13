int RGWGetObj_ObjStore_SWIFT::send_response_data_error()
{
  std::string error_content;
  op_ret = error_handler(op_ret, &error_content);
  if (! op_ret) {
    /* The error handler has taken care of the error. */
    return 0;
  }

  bufferlist error_bl;
  error_bl.append(error_content);
  return send_response_data(error_bl, 0, error_bl.length());
}