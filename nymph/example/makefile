nymph: nymph_compiler.c
	gcc -std=c11 nymph_compiler.c -o nymph
	./nymph rect.n rect
	./nymph box.n box
	./nymph main.n main
	gcc -std=c11 -c rect.c rect.h
	gcc -std=c11 -c box.c box.h
	gcc -std=c11 -c main.c main.h
	gcc -std=c11 main.o box.o rect.o -o out
