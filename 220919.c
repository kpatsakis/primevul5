int RGWPostObj_ObjStore_S3::get_tags()
{
  string tags_str;
  if (part_str(parts, "tagging", &tags_str)) {
    RGWXMLParser parser;
    if (!parser.init()){
      ldout(s->cct, 0) << "Couldn't init RGWObjTags XML parser" << dendl;
      err_msg = "Server couldn't process the request";
      return -EINVAL; // TODO: This class of errors in rgw code should be a 5XX error
    }
    if (!parser.parse(tags_str.c_str(), tags_str.size(), 1)) {
      ldout(s->cct,0 ) << "Invalid Tagging XML" << dendl;
      err_msg = "Invalid Tagging XML";
      return -EINVAL;
    }

    RGWObjTagging_S3 tagging;

    try {
      RGWXMLDecoder::decode_xml("Tagging", tagging, &parser);
    } catch (RGWXMLDecoder::err& err) {
      ldout(s->cct, 5) << "Malformed tagging request: " << err << dendl;
      return -EINVAL;
    }

    RGWObjTags obj_tags;
    int r = tagging.rebuild(obj_tags);
    if (r < 0)
      return r;

    bufferlist tags_bl;
    obj_tags.encode(tags_bl);
    ldout(s->cct, 20) << "Read " << obj_tags.count() << "tags" << dendl;
    attrs[RGW_ATTR_TAGS] = tags_bl;
  }


  return 0;
}