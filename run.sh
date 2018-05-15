#!/bin/sh

make

echo ''
build/genWitnessSolver data/test.txt
echo ''
build/genWitnessSolver data/puzzle2.txt
echo ''
build/genWitnessSolver data/puzzle3.txt
echo ''
build/genWitnessSolver data/lessLargePuzzle.txt