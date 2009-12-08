#!/bin/bash
# Replacement Processing start up script for Linux systems that
# enables the R for Processing library.

APPDIR=$(dirname $(readlink /proc/$$/fd/255))
RENV=$(R --vanilla --slave -e "cat(paste(R.home(),system.file (package='rJava'), system.file(package='R4P'),sep='\n'))")
R_HOME=$(echo "$RENV" | sed -n 1p)
RJAVA_HOME=$(echo "$RENV" | sed -n 2p)
R4P_HOME=$(echo "$RENV" | sed -n 3p)
if test -z "$R_HOME"; then
  echo "Error: R was not found in your PATH"
  exit 1
fi
if test -z "$RJAVA_HOME"; then
  echo "Error: The rJava package is not installed, please install it"
  exit 1
fi
if test -z "$R4P_HOME"; then
  echo "Error: The R4P package is not installed, please install it"
  exit 1
fi
export LD_LIBRARY_PATH="${R_HOME}/lib:${RJAVA_HOME}/libs:${RJAVA_HOME}/jri"
export R_HOME
CLASSPATH="${RJAVA_HOME}/jri/JRI.jar:${R4P_HOME}/lib/R4P.jar"

for LIB in \
    ${APPDIR}/java/lib/rt.jar \
    ${APPDIR}/java/lib/tools.jar \
    ${APPDIR}/lib/*.jar \
    ;
do
    CLASSPATH="${LIB}:${CLASSPATH}"
done
export CLASSPATH
export PATH="${APPDIR}/java/bin:${PATH}"

cd ${APPDIR}
java processing.app.Base
