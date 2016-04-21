more $1 | grep $2 | awk -v da=$3 -F" " '{print $da}' | awk '{print strtonum( "0x" $1 )}' > $4
gnuplot -e "outfile='$4.svg'" -e "infile='./$4'" gnuplotconf

