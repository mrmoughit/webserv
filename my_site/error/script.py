#!/usr/bin/env python3

import sys
block = "hello world\n" * 1000000  # ~12 MB
sys.stdout.write(block)