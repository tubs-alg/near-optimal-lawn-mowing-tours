import _mowing_bindings
def strategy_to_readable(strategy):
    if strategy == _mowing_bindings.INITIAL_STRATEGY_CH:
        return "convex_hull"
    elif strategy == _mowing_bindings.INITIAL_STRATEGY_VERTICES:
        return "vertices"
    elif strategy == _mowing_bindings.FOLLOWUP_STRATEGY_GRID:
        return "grid"
    elif strategy == _mowing_bindings.FOLLOWUP_STRATEGY_RANDOM:
        return "random"
    elif strategy == _mowing_bindings.FOLLOWUP_STRATEGY_SKELETON:
        return "skeleton"

    raise ValueError()
