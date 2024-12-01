This repository contains my parallel-programming labs. Fifth semester discipline.

Смотреть директорию Bellman!!!
На ветке devlop - код для OpenMP, на ветке main - не распраллеленный код.

Для запуска программы применяешь следующие команды(из директории bellman):
--devlop:
	gcc -c *.c
	gcc main.o -lreadline -fopenmp -o main
	./main data.txt

--main
	gcc -c *.c
	gcc main.o -lreadline -o main
	./main data.txt

В программе из файла data.txt создается полносвязный ориентированный граф на 
2500 вершин и 6247500 ребёр
После запуска нажимаешь седьмой пункт (find shortes path), далее вводишь:
	>1
	>1
	.....
	>50
	>50
	
Далее получишь вывод с кратчайшим путём и временем работы программы в секундах.

Чтобы в директиве OpenMP поменять количество потоков, поменяй в ветке devlop переменную
threads в функции bellman_ford.

