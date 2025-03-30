# Dining Philosphers Problem solved in C

## Problem's description

Problem ucztujących filozofów to klasyczny przykład w informatyce, pokazujący trudności z podziałem zasobów. Pięciu filozofów siedzi przy okrągłym stole, gdzie każdy albo je, albo rozmyśla. Żeby jeść, potrzebują dwóch sztućców – jednej z lewej i jednej z prawej strony. Jeśli każdy weźmie tylko jeden i będzie czekał na drugi, nikt nie będzie mógł jeść, co prowadzi do zakleszczenia (ang. deadlock). Prostymi rozwiązaniami tego problemu są metoda z hierarchią zasobów oraz wprowadzenie kelnera\arbitra. Zaimplementowałem opcję z kelnerem, który pozwala na ograniczenie ilości filozofów będących przy stole, a tym samym uniknięcie problemu wzięcia wszystkich szcztućców.

Analogie:
=========
Filozof - wątek

sztućce - ograniczone zasoby

Sposób rozwiązania sekcji krytycznych:
======================================


## Dependencies
There's no need to install any libraries. Only standard ones are being used.

## How to compile?
Use gcc compiler.
```C
gcc -o program.exe main.c
```
## How to run program?
Open CMD (Windows) in directory with program.exe file
```
main.exe n (under Windows)
```
where n is a number of philosphers
