# Архитектура вычислительных систем. ИДЗ №4 Вариант 15. Селезнев Григорий Ильич БПИ217.

## Вариант 15:
**Задача о клумбе.** На клумбе растет 40 цветов, за ними непрерывно следят
два садовника и поливают увядшие цветы, при этом оба садовника очень боятся полить один и тот же цветок, который еще не начал вянуть. Создать
многопоточное приложение, моделирующее состояния цветков на клумбе и действия садовников. 

## Как устроена программа:
> Для реализации моделирования этого процесса я использовал 3 потока.
> 1 поток для изменения состояния цветков
> 2 и 3 потоки для садовников (поливают цветы)

* Для запуска программы с использованием консольного ввода:
```
./program.exe
```
* Для запуска программы с использованием рандомизированного ввода:
```
./program.exe number
```
> Вместо number нужно ввести число (seed для рандомизации)
* Для запуска программы с использованием файлов 
```
./program.exe input.txt output.txt
```
> файл **input.txt** используется для получения данных, файл **output.txt** используется для вывода информации
> Оба файла должны находится рядом с **program.exe** (запускаемая программа)
