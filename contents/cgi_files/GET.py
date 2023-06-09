#!/usr/bin/python3
import sys

sys.stdout.write('Content-type: text/css\r\n')
sys.stdout.write('COntent-length: 25\r\n')
sys.stdout.write('\r\n')
sys.stdout.write('GET python\n')
sys.stdout.write('May the Force be with you.')