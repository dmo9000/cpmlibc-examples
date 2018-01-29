SEQUENCE=`seq -w 0000 0289`


for SEQ in $SEQUENCE; do 
	echo "$SEQ"
	convert -colorspace RGB -define png:color-type=3 $SEQ.chr.gif -write PNG8:$SEQ.png null:
	./pngrip $SEQ.png $SEQ.pf
	done

