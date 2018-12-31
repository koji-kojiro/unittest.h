#!/usr/bin/bash

readonly version="0.0.1"
readonly copy="(C) 2018 TANI Kojiro <kojiro0531@gmail.com>"
src='
##src##
'
prog=$(basename $0)
std=gnu11
macros=()
incdir=()
libs=()
nomain=false
cc=gcc

function prove-gcc-exists () {
  if ! type "$cc" >/dev/null 2>&1; then
    print-error "gcc: Command not found."
  fi
}

function print-error() {
  printf "\e[31mError:$1\e[m\n"
  exit 1
}

function message() {
  printf "\e[34m$1\e[m\n"
}


function show-version-and-exit () {
  echo "$prog" "$version"
  exit 1
}

function show-help-and-exit () {
  cat << EOS
Usage: $prog [options] file
Options:
  -D<macro>            define <macro>.
  -h, --help           show this help and exit.
  -I<path>             specify include path.
  -l<lib>              specify shared library to be linked.
  --no-main            tell compiler that no main function defined in files(s).
  -v, --version        show version info and exit.
EOS
  exit 1
}

function ccflags () {
  echo "${incdir[@]}" "${libs[@]}" "${macros[@]}" "-std=$std" -xc -
}

function set-macro () {
  if [[ -z "$1" ]]; then
    print-error "Missing arugument after \`-I\`."
  fi
  macros+=("-D$1")
}

function set-incdir () {
  if [[ -z "$1" ]]; then
    print-error "Missing arugument after \`-I\`."
  fi
  incdir+=("-I$1")
}

function set-lib () {
  if [[ -z "$1" ]]; then
    print-error "Missing arugument after \`-l\`."
  fi
  libs+=("-l$1")
}

function set-std () {
  if [[ -z "$1" ]]; then
    print-error "Missing argument after \`-std\`."
    exit 1
  fi
  if echo | "$cc" -std="$1" -fsyntax-only -xc - >/dev/null 2>&1; then
    std=$1
  else
    print-error "Unrecognized option for \`-std\`: $1"
    exit 1
  fi
}

function compile-and-run () {
  local bin=$(mktemp)
  local output="$(\
    echo "$src" | "$cc" $(ccflags) -o $bin \
    2>&1)"
  if [ -x $bin ]; then
    $bin
    local result="$(echo $?)"
    rm $bin
    exit $result
  else
    message "$(echo $output | grep -E error: | sort -u)"  
    print-error "Failed to compile."
  fi
}

function main () {
  prove-gcc-exists
  local opt
  for opt in "$@"; do
    case $opt in
      '-v' | '--version')
        show-version-and-exit
        ;;
      '-h' | '--help')
        show-help-and-exit
        ;;
      '-D')
        set-macro "$2"
        shift 2
        ;;
      -D*)
        set-macro "${1#-D}"
        shift 1
        ;;
      '-I')
        set-incdir "$2"
        shift 2
        ;;
      -I*)
        set-incdir "${1#-I}"
        shift 1
        ;;
     '-l')
        set-lib "$2"
        shift 2
        ;;
     -l*)
       set-lib "${1#-l}"
       shift 1
       ;;
     '-std')
       set-std "$2"
       shift 2
       ;;
     '-std=')
       set-std "${1#-std=}"
       shift 1
       ;;
     '--no-main')
       set-macro NOMAIN
       shift 1
       ;;
     -*)
       print-error "Unknown option: \`$1\`"
       ;;
     *)
       src+="$(cat $1)"
       shift 1
       compile-and-run
       ;;  
    esac
  done
  print-error "No input file."
}

main "$@"
