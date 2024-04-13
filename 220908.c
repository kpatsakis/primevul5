void RGWGetObjTags_ObjStore_S3::send_response_data(bufferlist& bl)
{
  dump_errno(s);
  end_header(s, this, "application/xml");
  dump_start(s);

  s->formatter->open_object_section_in_ns("Tagging", XMLNS_AWS_S3);
  s->formatter->open_object_section("TagSet");
  if (has_tags){
    RGWObjTagSet_S3 tagset;
    auto iter = bl.cbegin();
    try {
      tagset.decode(iter);
    } catch (buffer::error& err) {
      ldout(s->cct,0) << "ERROR: caught buffer::error, couldn't decode TagSet" << dendl;
      op_ret= -EIO;
      return;
    }
    tagset.dump_xml(s->formatter);
  }
  s->formatter->close_section();
  s->formatter->close_section();
  rgw_flush_formatter_and_reset(s, s->formatter);
}