build:
	mpic++ main.cpp -o main
run:
	mpirun  --oversubscribe -np 5 main
clear:
	rm main
