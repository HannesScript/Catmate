def cdb_to_dict(cdb):
    """
    Convert a CDB object to a dictionary.
    CDB is structed like this: FEN : UCI
    
    CDB Example:
    rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w : e2e4
    rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b : e7e5
    """
    
    result = {}
    for line in cdb.strip().splitlines():
        if not line.strip():
            continue
        try:
            fen, uci = line.split(":", 1)
        except ValueError:
            continue
        result[fen.strip()] = uci.strip()
    return result
    
def dict_to_cdb(dict):
    """
    Convert a dictionary to a CDB formatted string.
    Each key-value pair is converted to "FEN : UCI" on a separate line.
    """
    lines = [f"{fen} : {uci}" for fen, uci in dict.items()]
    return "\n".join(lines)
