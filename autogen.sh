#!/bin/sh
set -e
aclocal -Im4
autoheader
automake --add-missing --copy
autoconf
#./configure "$@"
echo "Now you are ready to run ./configure"
