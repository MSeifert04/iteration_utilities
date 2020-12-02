import pathlib
import shutil
import subprocess
import sys


if __name__ == '__main__':
    benchmark_directory = pathlib.Path('./benchmarks/')
    result_folder = pathlib.Path('./.benchmark_results/')

    # Create result folder and remove all png files there
    result_folder.mkdir(exist_ok=True)
    for path in result_folder.glob('**/*.png'):
        path.unlink()

    # Run the benchmarks
    benchmark_paths = list(benchmark_directory.glob('**/*.py'))
    print([str(path) for path in benchmark_paths])
    for path in benchmark_paths:
        inputfile = str(path)
        outputfile = str(result_folder.joinpath(path.with_suffix('.png').name))
        # Using sys.executables ensures that at least the same executable is used
        # as this file was executed. This avoid having to hardcode "python"
        # which may not work when only "python3" is present or a virtualenv
        # is used.
        subprocess.run([sys.executable, "-m", "simple_benchmark", inputfile, outputfile, '-v', '--time-per-benchmark', '0.05'], check=True)
