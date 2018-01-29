

#FONTS_TODO=`cat fonts.txt | sed -e "s/\s\(\s*\)\s/|\1|/g" | tr -s "|" |  awk -F'|' '$$3 == "Yes" { print $$1".pf"; }' | tr "\n" " "`

FONTS_TODO := $(shell cat fonts.txt | sed -e "s/\s\(\s*\)\s/|\1|/g" | tr -s "|" |  awk -F'|' '$$3 == "Yes" { print $$1".pf"; }' | tr "\n" " " )
all: pngrip unpack-fonts build-pf build-arc 

build-pf: 
	@make ${FONTS_TODO}
	@rm -f *.chr.gif
	@rm -f *.png

build-arc:
	@rm -f ../../oemfonts.arc
	arc a ../../oemfonts.arc ${FONTS_TODO} fonts.txt ../../bin/setfont.com ../../bin/getfont.com 1>/dev/null

unpack-fonts: 0289.chr.gif

0289.chr.gif:
	unzip -o fonts.zip 1>/dev/null 

pngrip: pngrip.c
	gcc -o pngrip pngrip.c -lpng -g -ggdb 

clean:
	rm -f *.gif *.png *.rip *.pf pngrip 

%.pf: %.chr.gif
	@convert -colorspace RGB -define png:color-type=3 $< -write PNG8:$@.png null: 
	@./pngrip $@.png $@ 1>/dev/null 2>&1 
	

