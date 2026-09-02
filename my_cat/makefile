CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99

# по умолчанию (прод-версия)
all: mycat

mycat: mycat.c
	$(CC) $(CFLAGS) -o mycat mycat.c

# для тестирования
test: mycat
	@echo "1. Создаем тестовый файл 'test_input.txt'..."
	@printf "первая строка\n\nтретья строка с пробелами   \nчетвертая строка без переноса" > test_input.txt
	@echo "   (Файл создан: 4 строки, включая одну пустую и одну без \\n в конце)"
	@echo "------------------------------------------"

	@echo "2. Тест 1a: ./mycat test_input.txt"
	@./mycat test_input.txt
	@echo "------------------------------------------"

	@echo "3. Тест 1b: ./mycat -n test_input.txt"
	@./mycat -n test_input.txt
	@echo "------------------------------------------"

	@echo "4. Тест 1c: ./mycat -nE test_input.txt"
	@./mycat -nE test_input.txt
	@echo "------------------------------------------"

	@echo "5. Тест 1d: ./mycat -bE test_input.txt"
	@./mycat -bE test_input.txt
	@echo "------------------------------------------"

	@echo "6. Тест 3b (туннелирование): ls -l | ./mycat -n"
	@ls -l | ./mycat -n
	@echo "------------------------------------------"

	@echo "7. Тест 3a (туннелирование): ./mycat test_input.txt | ./mycat -nE"
	@./mycat test_input.txt | ./mycat -nE
	@rm -f test_input.txt

# Очистка
clean:
	rm -f mycat test_input.txt