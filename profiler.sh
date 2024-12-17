gprof build/src/main build/src/gmon.out > analisys.txt
gprof2dot -w -s analisys.txt > analisys.dot
xdot analisys.dot