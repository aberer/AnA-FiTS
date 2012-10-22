#! /bin/sh

cat src/version.hpp  | cut -f 3 -d ' ' | tr -d '"'
