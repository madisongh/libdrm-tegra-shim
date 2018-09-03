#!/usr/bin/env python
#
# Hacky parser for the function prototypes
# in xf86drm.h and xf86drmMode.h to generate
# the macros used in libdrm-shim.c.
#

from __future__ import print_function
import re
import sys

fpat = re.compile(r'^([^ ]+) +([^ ]+) *(\(.*\));')

def strip_args(args):
    if args == '(void)' or args == '()':
        return '()'
    arglist = args[0:len(args)-1].split(',')
    strippedlist = []
    for arg in arglist:
        if '*' in arg:
            strippedlist.append(arg.split('*')[1])
        else:
            awords = arg.strip().split(' ')
            a = awords[len(awords)-1]
            if '[' in a:
                a = a[:a.index('[')]
            strippedlist.append(a)
    return '(' + ', '.join(strippedlist) + ')'

with open(sys.argv[1], 'r') as f:
    for line in f:
        m = fpat.match(line)
        if m is None:
            continue
        functype = m.group(1)
        funcname = m.group(2)
        funcargs = m.group(3)
        if functype == 'void':
            retfunc = 'return'
        elif funcname == 'drmOpen':
            retfunc = 'return -EINVAL'
        else:
            retfunc = 'return 0'
        print('    FUNCDEF(%s, %s, %s, %s, %s) \\' % (functype, funcname,
                                                      funcargs, strip_args(funcargs),
                                                      retfunc))
