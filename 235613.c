XMLNode::get_property(const char* name, std::string& value) const
{
	XMLProperty const* const prop = property (name);
	if (!prop)
		return false;

	value = prop->value ();

	return true;
}