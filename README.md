# Архитектура вычислительных систем. ИДЗ №4 Вариант 15. Селезнев Григорий Ильич БПИ217.

## Вариант 15:
*Задача о клумбе.* На клумбе растет 40 цветов, за ними непрерывно следят
два садовника и поливают увядшие цветы, при этом оба садовника очень боятся полить один и тот же цветок, который еще не начал вянуть. Создать
многопоточное приложение, моделирующее состояния цветков на клумбе и действия садовников. Для изменения состояния цветов создать
отдельный поток (а не 40 потоков для каждого цветка), который может
задавать одновременное начало увядания для нескольких цветков.

## Как устроена программа:
