bool HasFanoutValue(const FanoutsMap& fanouts, const FanoutsMap::iterator& it) {
  return it != fanouts.end() && !it->second.empty();
}