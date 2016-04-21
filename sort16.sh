#myvar=$(expr $1 + 4)
#echo $myvar
myvar=$(expr $3 + 1)
more $1 | grep $2 | awk -v da=$3 -v db=$myvar -F" " '{print $da$db}' | awk '{print strtonum( "0x" $1 )}' > $4
#more $1 | grep $2 | awk -F" " '{print $9}'
gnuplot -e "outfile='$4.svg'" -e "infile='./$4'" gnuplotconf
