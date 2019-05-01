all:
	gcc -o main main.c -lrt
	gcc -o lab02_ex3 lab02_ex3.c
clean:
	rm main
