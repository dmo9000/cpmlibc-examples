FORMAT=z80pack-4m
TARGET_DIR=/media/sf_OSZ/systems/df1a0966-a20a-47cd-a83a-a6c616cc550a/
DRIVE9_DSK=/media/sf_OSZ/systems/df1a0966-a20a-47cd-a83a-a6c616cc550a/Drive9.disk 
IMGDIRS := $(shell find /media/sf_OSZ/systems/* -type d ! -name "BP_*" 2>/dev/null )


all: clean drive9 Drive9.disk package install


package: 
	# binary files, then text files
	@for USER in `seq 0 15`; do															\
		if [ -d drive9/B/$$USER ]; then													\
			FILES=`ls -1 drive9/B/$$USER 2>/dev/null | tr "\n" " "` ;					\
			(( [ ! -z "$$FILES" ] && : ) || : ) ;										\
			for MYFILE in $$FILES ; do 													\
				echo "-> [BIN] $$USER:$$MYFILE" >> build.log	;										\
				cpmcp -f z80pack-4m Drive9.disk drive9/B/$$USER/$$MYFILE $$USER:	;	\
				done ;																	\
			fi	;																		\
		done	;																		\
    for USER in `seq 0 15`; do                                                         \
        if [ -d drive9/T/$$USER ]; then                                                 \
            FILES=`ls -1 drive9/T/$$USER 2>/dev/null | tr "\n" " "` ;                   \
            (( [ ! -z "$$FILES" ] && : ) || : ) ;                                       \
            for MYFILE in $$FILES ; do                                                  \
                echo "-> [TXT] $$USER:$$MYFILE" >> build.log;                            \
                cpmcp -f z80pack-4m -t Drive9.disk drive9/T/$$USER/$$MYFILE $$USER: ;   \
                done ;                                                                  \
            fi  ;                                                                       \
        done 



install:
	( [ -d "${TARGET_DIR}" ] && cp Drive9.disk ${DRIVE9_DSK} ) || exit 0
	@cp Drive9.disk ~/src/z80pack/z80pack-1.34/cpmsim/disks/library/drivej.dsk
	echo "IMGDIRS=[${IMGDIRS}]"
	if [ ! -z "${IMGDIRS}" ]; then										\
		for IMGDIR in ${IMGDIRS}; do														\
			cp Drive9.disk $${IMGDIR}/Drive9.disk	;										\
			done ;																							\
	fi


Drive9.disk:
	@cp ~/src/z80pack/z80pack-1.34/cpmsim/disks/library/drivei.dsk Drive9.disk
	#../mkdisk/mkdsk ./Drive9.disk
	mkfs.cpm -f z80pack-4m Drive9.disk 


clean:
	rm -rf ./drive9 Drive9.disk
	
drive9:
	rm -rf ./drive9
	mkdir -p ./drive9/T/{0,1,2,3,4}/
	mkdir -p ./drive9/B/{0,1,2,3,4}/

	@cp ../testdata/testfile.bin ./drive9/B/0/
	@cp ../utils/MBASIC.COM ./drive9/B/1/
	#@cp ../utils/BASCOM53.ZIP ./drive9/B/1/
	@cp ../htsrc/iotest.c ./drive9/T/1/
	@cp ../htsrc/hello.c ./drive9/T/1/
	@cp ../bin/tstansi.com ./drive9/B/0/
	@cp ../bin/lifegame.com ./drive9/B/0/
	@cp ../bin/cls.com ./drive9/B/0/
	@cp ../bin/cat.com ./drive9/B/0/
	@cp ../bin/telnet.com ./drive9/B/0/
	@cp ../bin/cpstest.com ./drive9/B/0/
	@cp ../bin/iotest.com ./drive9/B/0/
	@cp ../bin/malltest.com ./drive9/B/0/
	@cp ../bin/fstat.com ./drive9/B/0/
	@cp ../bin/format.com ./drive9/B/0/
	@cp ../bin/randtest.com ./drive9/B/0/
	@cp ../bin/getopt.com ./drive9/B/0/
#	@cp ../fonts/8x8/complete.txt ./drive9/T/4/
#	@cp ../fonts/8x8/*.pf ./drive9/B/4/
	@cp ../utils/unzip.com ./drive9/B/0
	@cp ../utils/unzip.com ./drive9/B/1
#	@cp ../bin/lzf.com ./drive9/B/0
#	@cp ../fonts/*.pf ./drive9/B/4/
#	@cp ../ansi/*.ans ./drive9/T/0/
#	@cp ../ansi/*.ans ./drive9/T/1/
	#@cp ../bin/readrip.com ./drive9/B/0/readrip.com
	#@cp ../bin/getfont.com ./drive9/B/4/getfont.com
	#@cp ../bin/setfont.com ./drive9/B/4/setfont.com
	cp ../*.arc ./drive9/B/0/
	#@cp ../oempkg.arc ./drive9/B/0/oempkg.arc
	#@cp ../oemfonts.arc ./drive9/B/0/oemfonts.arc
	#@cp ../oemimage.arc ./drive9/B/0/oemimage.arc
	#@cp ../utils/WS4.ARK ./drive9/B/0/WS4.ARC
	@cp ../utils/unarc.com ./drive9/B/0/unarc.com
	@cp ../utils/unarc.com ./drive9/B/4/unarc.com
	@cp ../utils/MORE.COM ./drive9/B/0/MORE.COM
	@cp ../utils/MORE.COM ./drive9/B/4/MORE.COM
	#@cp ../pngrip/*.rip ./drive9/B/0/
	@cp ../testpack/*.* ./drive9/B/3
	#@cp ../games/advent.* ./drive9/B/2
	#@cp ../utils/dstat80.com ./drive9/B/0
	@cp ../games/roguevt.com ./drive9/B/2
	#@cp ../games/gorilla.com ./drive9/B/0
	#@cp ../games/hangman.com ./drive9/B/0
	#@cp ../games/words.txt ./drive9/T/0
	#cp ../utils/pmautoae.com ./drive9/B/0

	#@cp ../ansifiles/* ./drive9/T/1
	@cp ../utils/termtest.com ./drive9/B/0
	@cp ../utils/ansitest.com ./drive9/B/0
	@cp ../text/test[0-9].txt drive9/T/0
	@cp ../utils/stat.com drive9/B/0
	@cp ../utils/sdir.com drive9/B/0
	@cp ../utils/bye.com drive9/B/0
	@cp ../utils/bye.com drive9/B/1
	@cp ../utils/bye.com drive9/B/2
	@cp ../utils/pip.com drive9/B/0
	@cp ../utils/pip.com drive9/B/1
	@cp ../utils/pip.com drive9/B/2
	@cp ../utils/grep.com drive9/B/1
	#@cp ../utils/QRUN.COM drive9/B/0
	#@cp ../utils/DIFF.COM drive9/B/0
	#@cp ../testsrc/hello.c drive9/T/1
	#@cp ../testsrc/md5.c drive9/T/1
	#sed -e "s/^.*\/\/.*$$//g" ../testsrc/md5.c > drive9/T/1/md5.c
	@cp ../utils/TE.COM drive9/B/0
	#@cp ../utils/TE.COM drive9/B/1
	@cp ../utils/dump.com drive9/B/0
	@cp ../utils/dump.com drive9/B/1
	# USER 2
	@cp ../zork/*.COM drive9/B/2
	@cp ../zork/*.DAT drive9/B/2
	cd ./drive9/B/1 && lha e ../../../../utils/z80v309.lzh 1>/dev/null 2>&1
	mv ./drive9/B/1/*.h ./drive9/T/1/


