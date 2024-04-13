HeaderTableRecord::HeaderTableRecord(const char *n, HdrType theId, HdrFieldType theType, int theKind) :
    name(n), id(theId), type(theType),
    list(theKind & HdrKind::ListHeader), request(theKind & HdrKind::RequestHeader),
    reply(theKind & HdrKind::ReplyHeader), hopbyhop(theKind & HdrKind::HopByHopHeader),
    denied304(theKind & HdrKind::Denied304Header)
{}