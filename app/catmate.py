import subprocess

def run(executable_path: str, fen_string: str, depth: int, database: str) -> str:
    try:
        result = subprocess.run([executable_path, fen_string, str(depth), database], capture_output=True, text=True)
        return result.stdout.strip()
    except Exception as e:
        print(f"Error running Catmate: {e}")
        return ""
