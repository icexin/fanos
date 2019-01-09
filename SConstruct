#!/usr/bin/env python

#encoding:utf8
import subprocess
import signal
import sys
import os

def error_colorize(message):
    _ERRORS = [': error:', ': fatal error:', ': undefined reference to',
           ': cannot find ', ': ld returned 1 exit status',
           ' is not defined'
           ]
    _WARNINGS = [': warning:', ': note: ', '] Warning: ']

    _colors = {}
    _colors['red']    = '\033[1;31m'
    _colors['green']  = '\033[1;32m'
    _colors['yellow'] = '\033[1;33m'
    _colors['cyan']   = '\033[1;36m'
    _colors['end']    = '\033[0m'

    colored_message = []
    for t in message.splitlines(True):
        color = 'cyan'

        # For clang column indicator, such as '^~~~~~'
        if t.strip().startswith('^'):
            color = 'green'
        else:
            for w in _WARNINGS:
                if w in t:
                    color = 'yellow'
                    break
            for w in _ERRORS:
                if w in t:
                    color = 'red'
                    break

        colored_message.append(_colors[color])
        colored_message.append(t)
        colored_message.append(_colors['end'])
    return ''.join(colored_message)

# 给gcc的输出加上颜色
def echospawn(sh, escape, cmd, args, env):
    # convert env from unicode strings
    asciienv = {}
    for key, value in env.iteritems():
        asciienv[key] = str(value)

    cmdline = ' '.join(args)
    p = subprocess.Popen(
        cmdline,
        env=asciienv,
        stderr=subprocess.PIPE,
        stdout=subprocess.PIPE,
        shell=True,
        universal_newlines=True)
    (stdout, stderr) = p.communicate()

    if p.returncode:
        if p.returncode != -signal.SIGINT:
            # Error
            sys.stdout.write(error_colorize(stdout))
            sys.stderr.write(error_colorize(stderr))
    else:
        if stderr:
            # Only warnings
            sys.stdout.write(error_colorize(stdout))
            sys.stderr.write(error_colorize(stderr))
        else:
            sys.stdout.write(stdout)

    return p.returncode

CFLAGS = [
    '-nostdlib',
    '-nostdinc',
    '-fno-builtin',
    '-fno-stack-protector',
    '-g',
    '-Wall',
    #'-Werror',
    '-DDEBUG',
    '-Werror=implicit-function-declaration'
]

env = Environment(ENV = os.environ)
env['SPAWN'] = echospawn
env['CC'] = "i386-elf-gcc"
env['AR'] = "i386-elf-ar"
env['RANLIB'] = 'i386-elf-ranlib'
env.VariantDir('build', 'src')

env.Library('fanos',
    Glob('build/fanos/*.c') + Glob('build/fanos/*.S'),
            CFLAGS=CFLAGS, CPPPATH=['src'])
