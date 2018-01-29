#!/bin/sh
find . -name "*.lst" -exec grep -Hr "^   $1" {} \;
