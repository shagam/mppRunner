
mppRunner: mppRunner.c args.c args.h Makefile
	g++ -g -o4  -std=c++0x  -o mppRunner mppRunner.c args.c -lpthread
	
java:
	java -jar -ea ~/nb/mpprunner/dist/mpprunner.jar  thr=32 iter=1000000

c:
	~/nb/mpprunner/src/mpprunner/mppRunner thr=32 loops=1000000 	

