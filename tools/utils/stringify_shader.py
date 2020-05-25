#!/usr/bin/env python3

import sys
import os

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage " + sys.argv[0] + " shader file")
        sys.exit()
    with open(sys.argv[1], 'r') as f:
    	lines = [line[0:-1] for line in f.readlines()]
    	print("\"" + '\\n'.join(lines) + "\"")
