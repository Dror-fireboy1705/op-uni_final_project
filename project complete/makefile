all: assembler
assembler: assembler.c pre_assembler.c run_first.c run_second.c assist.c macro_list.c symbl_list.c word_arrays.c Elbl_list.c assembler.h
	gcc -ansi -pedantic -g -Wall assembler.c pre_assembler.c run_first.c run_second.c assist.c macro_list.c symbl_list.c word_arrays.c Elbl_list.c assembler.h -o assembler
