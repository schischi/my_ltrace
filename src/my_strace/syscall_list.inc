{ E_INT, "read", 0, 3, { {E_INT, "fd"}, {E_STR, "buf"}, {E_UINT, "count"} } },
{ E_INT, "write", 1, 3, { {E_INT, "fd"}, {E_STR, "buf"}, {E_UINT, "count"} } },
{ E_INT, "open", 2, 2, { {E_STR, "pathname"}, {E_INT, "flags"} }},
{ E_INT, "close", 3, 1, { {E_INT, "fd"} } },
