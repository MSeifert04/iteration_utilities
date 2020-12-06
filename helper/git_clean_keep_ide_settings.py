import subprocess

# Keep the following directories
# - .vscode (Visual Studio code project settings)
# - .vs (Visual Studio project files)
# - .idea (PyCharm project files)
subprocess.run(["git", "clean", "-dfx", "-e", ".vscode", "-e", ".idea", "-e", ".vs"], check=True)
