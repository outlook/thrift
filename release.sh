#!/usr/bin/env bash -e
#
# NAME
#   release.sh
#
# SYNOPSIS
#   release.sh -- build and stage a release of the Thrift compiler in Outlook/thrift.
#
# DESCRIPTION
#   Compiles Thrift using 'make', then creates a draft release in Github
#   and uploads the compiler binary as an asset.  Releases must be manually
#   published.
#
#   Releases are named for the current date and time, prefixed with 'ol-'.
#
# REQUIREMENTS
#   Credentials are required; this script expects the file ~/.gh_token
#   to contain them.  The file must contain a single line of the form
#   [github_username]':'[github_access_token], i.e. ben:abcdefg.
#
#   It is expected that the user has manually built Thrift at least once,
#   and specifically that './configure' has been used successfully to generate
#   a working build configuration.  For more details, see README.md.

test -f ~/.gh_token || exit 1

function trim() {
  echo "$1" | tr -d '[[:space:]]'
}

SCRIPT_DIR=$(trim $( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd ))
THRIFT_COMPILER=`trim "$SCRIPT_DIR/compiler/cpp/thrift"`

pushd "$SCRIPT_DIR"
trap popd EXIT

CREDS=$(head -n 1 ~/.gh_token)
TAG=ol-$(date +%Y%m%d%H%M%S)
NAME=$(trim $(echo "$CREDS"  | cut -d ':' -f 1))
TOKEN=$(trim $(echo "$CREDS" | cut -d ':' -f 2))

if [[ -z "$NAME" || -z "$TOKEN" ]]; then
  echo "~/.gh_token required; must contain <username>:<token>"
  exit 1
fi

make

# Doing this all via cURL would be just too painful, so we use
# a companion Python script to do that HTTP legwork.
python upload_compiler_to_github.py "$TAG" "$NAME" "$TOKEN" "$THRIFT_COMPILER"

