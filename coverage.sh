#!/bin/sh

lcov --directory . -c -o report.info
lcov --remove report.info '*Qt*' '/usr/include/*' '*/qt3d/*' '*/kuesa/examples/*' '*/kuesa/tests/auto/*' '*/kuesa/src/core/iro-materials/*' '*/kuesa/src/3rdparty/*' 'compression/*' 'compression' 'core' 'io' 'mesh' 'point_cloud' 'metadata' -o report-filtered.info
genhtml report-filtered.info --output-directory coverage
