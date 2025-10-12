#!/bin/sh -e

BASEDIR=$(realpath ""$(dirname "$0")"")
ROOTDIR="$BASEDIR/.."
BINDIR=$(realpath "$ROOTDIR/build/src")
TESTDIR=$(realpath "$ROOTDIR/tests")

echo "Test comparer"
python3 "${TESTDIR}/comparer/comparer_checker.py" "${BINDIR}/comparer" "${TESTDIR}/comparer/test_data"

echo "Test converter"
python3 "${TESTDIR}/converter/converter_checker.py" "${BINDIR}/converter" "${TESTDIR}/converter/test_data" "${BINDIR}/comparer"
