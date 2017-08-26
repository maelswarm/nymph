nymph: nymph_pp.c main.n box.n rect.n
	gcc -std=c11 -c helper_func.c helper_func.h
	gcc -std=c11 -c error_checking.c error_checking.h
	gcc -std=c11 -c nymph_pp.c
	gcc -std=c11 helper_func.o error_checking.o nymph_pp.o -o nymph
	./nymph main.n main
	gcc -std=c11 -c rect.c rect.h
	gcc -std=c11 -c box.c box.h
	gcc -std=c11 -c main.c main.h
	gcc -std=c11 main.o box.o rect.o -o out
