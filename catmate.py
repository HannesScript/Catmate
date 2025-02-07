import subprocess

executable = ''

def run(fen_string: str, depth: int, database: str) -> str:
    try:
        result = subprocess.run([executable, f"\"{fen_string}\" {depth} \"{database}\""])    # This process will run the executable with the given arguments, which prints out something like "e2e4" to stdout
        return result.stdout.decode('utf-8').strip()    # This will return the output of the process
    except Exception as e:
        print(f"Error running Catmate: {e}")
        return ""
