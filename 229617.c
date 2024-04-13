HeaderTableRecord::HeaderTableRecord() :
    name(""), id(HdrType::BAD_HDR), type(HdrFieldType::ftInvalid),
    list(false), request(false), reply(false), hopbyhop(false), denied304(false)
{}