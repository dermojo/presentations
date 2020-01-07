#!/bin/bash

# run once:
# for f in *.puml; do plantuml -tsvg $f; done

THEME=${1:-white}
pandoc -t revealjs -s -o Overview.html Overview.md -V revealjs-url=./reveal.js -V theme=$THEME
