#!/bin/bash

LLVM_URL="https://github.com/gaps-closure/capo/releases/download/T0.1/LLVM-10.0.0svn-Linux.deb"
EMU_ARM_URL="https://github.com/gaps-closure/emu/releases/download/v1.0/ubuntu-arm64-goldenimage.tgz"

RELEASES=($LLVM_URL $EMU_ARM_URL)
BIN=bin

usage_exit() {
  [[ -n "$1" ]] && echo $1
  echo "Usage: $0 [ -vsh ] \\"
  echo "-o  Ownship"
  echo "-t  Target"
  echo "-p  Output to PNG"
  echo "-l <TITLE>"
  echo "-h  Help"
  exit 1
}

handle_opts() {
  local OPTIND
  while getopts "l:opth" options; do
    case "${options}" in
      o) OWNSHIP=1  ;;
      t) TARGET=1                ;;
      p) PNG=1                   ;;
      l) TITLE=${OPTARG}  ;;
      h) usage_exit "Help"       ;;
      :) usage_exit "Error: -${OPTARG} requires an argument." ;;
    esac
  done
  
  shift $((OPTIND -1))
  components=($@)
}

gen_plots() {
    local TYPE=$1

    local CMD="plot "

    COORDS=(X Y Z)
    COORDS_PART=(X-part Y-part Z-part)

    for col in {2..4}
    do
        let i=$col-2;
        CMD+="\"../pnt-example-corr/${TYPE}.txt\" using 2:$col title \"${COORDS[${i}]}\", "
    done

    for col in {2..4}
    do
        let i=$col-2;
        CMD+="\"${TYPE}-part.txt\" using 2:$col title \"${COORDS_PART[${i}]}\", "
    done

    FILE="plots-${TYPE}"


    if [[ $PNG ]]; then
        echo "set terminal png" > $FILE
        echo "set output '${TYPE}.png'" >> $FILE
    else
        echo > $FILE
    fi

    if [[ $TITLE ]]; then
        echo "set title '$TITLE'" >> $FILE
    elif [[ $OWNSHIP ]]; then
        echo "set title 'UAV Tracks'" >> $FILE
    elif [[ $TARGET ]]; then
        echo "set title 'Target Tracks'" >> $FILE
    fi

    echo $CMD >> $FILE
}

handle_opts "$@"

if [[ $OWNSHIP ]]; then
    gen_plots ownship
elif [[ $TARGET ]]; then
    gen_plots target
fi
gnuplot -p -c $FILE

