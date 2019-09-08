import hashlib
import sys

if __name__ == '__main__':
    filename = sys.argv[1]
    required_md5 = sys.argv[2]
    with open(filename, 'rb') as f:
        file_md5 = hashlib.md5(f.read()).hexdigest()
    print('filename: ', filename)
    print('file md5: ', file_md5)
    print('req. md5: ', required_md5)
    print('match:   ', file_md5 == required_md5)
    assert file_md5 == required_md5
