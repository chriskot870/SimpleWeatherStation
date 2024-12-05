#!/usr/bin/bash

pkgs=`awk '
  BEGIN {OFS=" " }
  {
  if (NF == 4)
    if ($4 ~ installed && $4 !~ /automatic/) {
      split($1,name, "/");
      print name[1];
    }
  }' < $1`

# If there are any pkgs then do an apt install
if [[ -z $pkgs ]]
then
  echo No packages to load
else
  sudo apt -y install $pkgs
fi
