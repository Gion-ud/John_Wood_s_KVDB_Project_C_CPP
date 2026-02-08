#!/usr/bin/env python3
import uuid
import sys


RED = '\x1b[31m'
GREEN = '\x1b[32m'
RESET = '\x1b[0m'

def main(argv):
    print('UUID Gennerator')
    argc = len(argv)
    if (argc != 3):
        print(RED + 'Error: argc != 3' + RESET)
        print('usage: <prog name> <output file path> <uuid count>')
        sys.exit(1)
    
    if (not argv[2].isdigit()):
        printf(RED + 'Error: argc[2] (a.k.a. uuid count) must be a positive integer' + RESET)
        sys.exit(1)

    ftxt = open(argv[1], "w")
    i = 0
    uuid_cnt = int(argv[2])
    while (i < uuid_cnt):
        u = uuid.uuid4()
        ftxt.write(str(u) + '\n')
        i += 1

    ftxt.close()
    print(GREEN + f"{i} counts of uuid generated" + RESET)

if (__name__ == '__main__'):
    main(sys.argv)
