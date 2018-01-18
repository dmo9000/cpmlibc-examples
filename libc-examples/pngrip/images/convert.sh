convert -colorspace RGB -resize 320x200\! -gamma 1.0 -colors 255 -define png:color-type=3 -dither Riemersma $1 $1.png
