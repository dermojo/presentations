#!/bin/bash
THEME=${1:-white}
pandoc -t revealjs -s -o Overview.html Overview.md -V revealjs-url=./reveal.js -V theme=$THEME
