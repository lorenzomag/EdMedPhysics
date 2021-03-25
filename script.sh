#!/bin/bash
pid_counter=0

if [[ $# == 0 ]]; then
    PARTICLES="gammas neutrons"
    echo "Running plot_same"
    root root_macros/plot_same.C 1> /dev/null &   pids[$((pid_counter++))]=$!
else
    PARTICLES=$*
fi

for PARTICLE in $PARTICLES; do
    echo "Running analyse_dose for $PARTICLE"
    root "root_macros/analyse_dose.C(\"$PARTICLE\")" 1> /dev/null &  pids[$((pid_counter++))]=$!
    echo "Running make_histos for $PARTICLE"
    root "root_macros/make_histos.C(\"$PARTICLE\")" 1> /dev/null &  pids[$((pid_counter++))]=$!
    echo "Running edit_histo for $PARTICLE"
    root "root_macros/edit_histo.C(60,\"$PARTICLE\")" 1> /dev/null &    pids[$((pid_counter++))]=$!    
done


# wait for all pids
for pid in ${pids[*]}; do
    wait $pid
done

mkdir -p figures
for PARTICLE in $PARTICLES; do
	echo "Moved files to ${PARTICLE}_output/"
	mkdir -p ${PARTICLE}_output
	mv ${PARTICLE}*.root ${PARTICLE}_output/
    mv ${PARTICLE}*.pdf figures
done

