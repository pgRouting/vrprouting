SELECT * FROM vrp_oneDepot(
    'SELECT * FROM onedepot.orders',
    'SELECT * FROM onedepot.vehicles',
    'SELECT * FROM onedepot.distances',
    1);
