#!/bin/bash

if [[ $# == 0 ]]; then
    PARTICLES="gammas neutrons"
else
    PARTICLES=$*
fi

pid_counter=0
for PARTICLE in $PARTICLES; do
    echo "Running analyse_dose for $PARTICLE"
    root "root_macros/analyse_dose.C(\"$PARTICLE\")" 1> /dev/null &  pids[$((pid_counter++))]=$!
    echo "Running make_histos for $PARTICLE"
    root "root_macros/make_histos.C(\"$PARTICLE\")" 1> /dev/null &  pids[$((pid_counter++))]=$!
    echo "Running edit_histo for $PARTICLE"
    root "root_macros/edit_histo.C(10,\"$PARTICLE\")" 1> /dev/null &    pids[$((pid_counter++))]=$!    
done

echo "Running plot_same"
root root_macros/plot_same.C 1> /dev/null &   pids[$((pid_counter++))]=$!

# wait for all pids
for pid in ${pids[*]}; do
    wait $pid
done

echo "Moved files to output/"
mkdir -p output
mv *.root *.pdf output/