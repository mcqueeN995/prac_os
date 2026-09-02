CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99

# по умолчанию (прод-версия)
all: mygrep

mygrep: mygrep.c
	$(CC) $(CFLAGS) -o mygrep mygrep.c

# для тестирования
test: mygrep
	@echo "1. Создаем тестовый файл 'test_input.txt'..."
	@printf "apple\nbanana\ncherry\napple pie\ngrape\n" > test_input.txt
	@echo "   (Файл создан: 5 строк с разными фруктами)"
	@echo "------------------------------------------"

	@echo "2. Тест 2a: ./mygrep apple test_input.txt"
	@./mygrep apple test_input.txt
	@echo "------------------------------------------"

	@echo "3. Тест 2b: ./mygrep banana test_input.txt"
	@./mygrep banana test_input.txt
	@echo "------------------------------------------"

	@echo "4. Тест 2c: ./mygrep xyz test_input.txt (ничего не найдено)"
	@./mygrep xyz test_input.txt
	@echo "(Если ничего не выведено - это правильно)"
	@echo "------------------------------------------"

	@echo "5. Тест 3a (туннелирование): cat test_input.txt | ./mygrep apple"
	@cat test_input.txt | ./mygrep apple
	@echo "------------------------------------------"

	@echo "6. Тест 3b (туннелирование): ls -l | ./mygrep mygrep"
	@ls -l | ./mygrep mygrep
	@rm -f test_input.txt

# Очистка
clean:
	rm -f mygrep test_input.txt