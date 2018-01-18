SIZE=`stat -c "%s" $1`
#PADDED_BYTES=`echo "( ( $SIZE / 128 ) + 1 ) * 128" | bc`
PADDED_BYTES=`echo $SIZE + 1 | bc `
echo "SIZE=$SIZE"
echo "PAD_TO_SIZE=$PADDED_BYTES"
while true; do echo -ne '\x1e'; done | dd of=$2 bs=1 count=$PADDED_BYTES 
dd if=$1 of=$2 conv=notrunc
