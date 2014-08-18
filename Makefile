buildrun:
	rm -rf bin
	mkdir bin
	gcc -g src/main.c src/utility.c src/bitmap.c src/lobjder.c src/motion.c -lglut -lGL -lGLU -lm -lGLEW -o bin/fin -Wall
	./bin/fin
build:
	rm -rf bin
	mkdir bin
	gcc -g src/main.c src/utility.c src/bitmap.c src/lobjder.c src/motion.c -lglut -lGL -lGLU -lm -lGLEW -o bin/fin -Wall
run:
	./bin/fin
clear:
	rm -rf bin
