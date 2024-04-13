void SwapFanoutsMapValues(FanoutsMap* fanouts,
                          const MutableGraphView::OutputPort& from_port,
                          const FanoutsMap::iterator& from_fanouts,
                          const MutableGraphView::OutputPort& to_port,
                          const FanoutsMap::iterator& to_fanouts) {
  const bool from_exists = from_fanouts != fanouts->end();
  const bool to_exists = to_fanouts != fanouts->end();

  if (from_exists && to_exists) {
    std::swap(from_fanouts->second, to_fanouts->second);
  } else if (from_exists) {
    fanouts->emplace(to_port, std::move(from_fanouts->second));
    fanouts->erase(from_port);
  } else if (to_exists) {
    fanouts->emplace(from_port, std::move(to_fanouts->second));
    fanouts->erase(to_port);
  }
}