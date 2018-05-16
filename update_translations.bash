#! /bin/bash

set -e
set -o pipefail

function update_translation {
    local f="$( basename "${1%.*}" )"
    lupdate -source-language en_US -target-language "${f#*.}" -no-obsolete -locations relative "$( dirname "$1" )" "$1" -ts "$1"
}

function release_translation {
    lrelease -markuntranslated "Not translated!" -nounfinished -removeidentical -compress "$1" -qm "${1%.*}.qm"
}

cd "$( dirname "$0" )"

shopt -s globstar
for f in **/*.ts
do
    update_translation "$f"
    linguist "$f"
    release_translation "$f"
    echo
done

