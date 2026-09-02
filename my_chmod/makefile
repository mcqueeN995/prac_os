CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99

all: mychmod


# по умолчанию (прод-версия)
mychmod: mychmod.c
	$(CC) $(CFLAGS) -o mychmod mychmod.c

# для тестирования
test: mychmod
	@echo "1. Создаем тестовый файл..."
	@rm -f test_file.txt
	@touch test_file.txt
	@chmod 644 test_file.txt
	@echo "   Начальные права:"
	@ls -l test_file.txt
	@echo "------------------------------------------"

	@echo "2. Тест a: ./mychmod +x test_file.txt"
	@./mychmod +x test_file.txt
	@ls -l test_file.txt
	@echo "------------------------------------------"

	@echo "3. Восстанавливаем права..."
	@chmod 644 test_file.txt
	@echo "4. Тест b: ./mychmod u-r test_file.txt"
	@./mychmod u-r test_file.txt
	@ls -l test_file.txt
	@echo "------------------------------------------"

	@echo "5. Восстанавливаем права..."
	@chmod 644 test_file.txt
	@echo "6. Тест c: ./mychmod g+rw test_file.txt"
	@./mychmod g+rw test_file.txt
	@ls -l test_file.txt
	@echo "------------------------------------------"

	@echo "7. Восстанавливаем права..."
	@chmod 644 test_file.txt
	@echo "8. Тест d: ./mychmod ug+rw test_file.txt"
	@./mychmod ug+rw test_file.txt
	@ls -l test_file.txt
	@echo "------------------------------------------"

	@echo "9. Восстанавливаем права..."
	@chmod 644 test_file.txt
	@echo "10. Тест e: ./mychmod uga+rwx test_file.txt"
	@./mychmod uga+rwx test_file.txt
	@ls -l test_file.txt
	@echo "------------------------------------------"

	@echo "11. Тест f: ./mychmod 766 test_file.txt"
	@./mychmod 766 test_file.txt
	@ls -l test_file.txt
	@echo "------------------------------------------"

	@echo "12. Тест: несколько файлов"
	@touch test_file2.txt
	@chmod 644 test_file2.txt
	@./mychmod 755 test_file.txt test_file2.txt
	@ls -l test_file.txt test_file2.txt
	@echo "------------------------------------------"

	@echo "13. Сравнение с оригинальным chmod:"
	@chmod 644 test_file.txt
	@echo "--- chmod g+rw test_file.txt ---"
	@chmod g+rw test_file.txt
	@ls -l test_file.txt
	@chmod 644 test_file.txt
	@echo "--- ./mychmod g+rw test_file.txt ---"
	@./mychmod g+rw test_file.txt
	@ls -l test_file.txt
	@rm -f test_file.txt test_file2.txt

clean:
	rm -f mychmod test_file.txt test_file2.txt