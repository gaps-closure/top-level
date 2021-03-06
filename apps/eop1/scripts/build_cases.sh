#!/bin/bash

HWMODE=
DEVFILE=
PWD=`pwd`

usage_exit() {
  [[ -n "$1" ]] && echo $1
  echo "Usage: $0 [ -mh ]"
  echo "-h          Help"
  echo "-m          Hardware Mode (ilip, mind, pl, emu)"
  exit 1
}

handle_opts() {
  local OPTIND
  while getopts "im:h" options; do
      case "${options}" in
	  m) HWMODE=${OPTARG}       ;;
	  h) usage_exit             ;;
	  :) usage_exit "Error: -${OPTARG} requires argument." ;;
	  *) usage_exit
      esac
  done
  shift "$((OPTIND-1))"

  PWD=`pwd`
  PINSTALL="${PWD}/../case1/MA_v1.0_src/pinstall"

  case $HWMODE in
      ilip)
	  DEVFILE=/opt/closure/etc/devices_eop_ilip_v3.json
	  ;;
      pl)
	  DEVFILE=/opt/closure/etc/devices_eop_pl.json
	  ;;
      mind)
	  DEVFILE=/opt/closure/etc/devices_eop_mind.json
	  ;;
      emu)
	  DEVFILE=/opt/closure/etc/devices_socat.json
	  ;;
      *)
	  usage_exit
  esac
}

runtask() {
    PINSTALL=$PINSTALL HWMODE=$HWMODE vstask "$@"
}

build_xdcc() {
    runtask "INIT"
    runtask "0 CLEAN SOURCE"
    runtask "1 ANNOTATE"
    runtask "2 ANALYZE PARTITION CONFLICTS"
    # vstask does not support dependsOn field in tasks.json
    runtask "9a DIVIDE"
    runtask "9b AUTOGEN GEDL, RPCs, IDL, Codecs"
    runtask "9c VERIFY"
    runtask "9e BUILD"
}

handle_opts "$@"

for CASE in case1 case2 case3
do
    pushd ../$CASE/design
    bash README.md
    popd
    pushd ../$CASE/xdcc/egress_xdcc
    build_xdcc
    popd
    pushd ../$CASE/xdcc/ingress_xdcc
    build_xdcc
    popd
    pushd ../$CASE/deploy
    runtask "3 HALDEPLOY"
    python3 /opt/closure/scripts/hal_autoconfig.py -o . -x ./xdconf.ini -d $DEVFILE -p hal_$CASE
    popd
done
