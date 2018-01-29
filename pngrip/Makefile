
pngtest: pngtest.c
	gcc -o pngtest pngtest.c -lpng
	# try to remove dodgy sRGB color profiles
#	mogrify *.png
	./pngtest forest.png forest2.png
	mv testimg.rip forest.rip
	./pngtest headroom.png headroom2.png
	mv testimg.rip headroom.rip
	./pngtest cprogs.png cprogs2.png
	mv testimg.rip cprogs.rip
	./pngtest buick.png buick2.png
	mv testimg.rip buick.rip
	./pngtest sunrise.png sunrise2.png
	mv testimg.rip sunrise.rip
	./pngtest canyon.png canyon2.png
	mv testimg.rip canyon.rip
	./pngtest alien.png alien2.png
	mv testimg.rip alien.rip
	./pngtest train.png train2.png
	mv testimg.rip train.rip
	./pngtest colormap.png colormap2.png
	mv testimg.rip colormap.rip
	./pngtest qrddbf.png qrddbf2.png 
	mv testimg.rip qrddbf.rip 


clean:
	rm -f pngtest
	rm -f *.rip
	rm -f egypt2.png
