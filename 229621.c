operator<< (std::ostream &s, Http::HdrType id)
{
    if (Http::any_HdrType_enum_value(id))
        s << Http::HeaderLookupTable.lookup(id).name << '[' << static_cast<int>(id) << ']';
    else
        s << "Invalid-Header[" << static_cast<int>(id) << ']';
    return s;
}