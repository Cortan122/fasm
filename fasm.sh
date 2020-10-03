#!/bin/bash

arg2="$2"
[ "$2" == "" ] && arg2="run.exe"
[ "$2" == "--run" ] && arg2="run.exe"
[ "$2" == "--cat" ] && cat "$1" && exit
[ "$2" == "--code" ] && cmd.exe /C "code.cmd" "$(wslpath -am "$1")" && exit

touch "$arg2"
in="$(wslpath -am -- "$1")"
out="$(wslpath -am -- "$arg2")"
realout="$(realpath -- "$arg2")"
rm "$arg2"

cd ~/Downloads/fasmw17325/INCLUDE/
../FASM.EXE "$in" "$out"
cd - >/dev/null

[ "$2" == "--run" ] && "$realout" "${@:3}"
