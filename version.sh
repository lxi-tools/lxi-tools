#!/bin/sh

if [ "$1" = "get-vcs" ]; then
  git -C "$MESON_SOURCE_ROOT" describe --always --dirty | sed 's/^v//'
elif [ "$1" = "set-dist" ]; then
  $MESONREWRITE --sourcedir="$MESON_PROJECT_DIST_ROOT" kwargs set project / version "$2"
else
  exit 1
fi
