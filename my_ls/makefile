CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99

all: myls

# по умолчанию (прод-версия)
myls: myls.c
	$(CC) $(CFLAGS) -o myls myls.c

# для тестирования
test: myls
	@mkdir -p test_dir
	@touch test_dir/file1.txt
	@touch test_dir/file2.txt
	@touch test_dir/.hidden
	@mkdir test_dir/subdir
	@touch test_dir/executable
	@chmod +x test_dir/executable
	@ln -sf file1.txt test_dir/link_to_file
	@echo "1. Тест: ./myls test_dir"
	@./myls test_dir
	@echo "------------------------------------------"
	@echo "2. Тест: ./myls -a test_dir"
	@./myls -a test_dir
	@echo "------------------------------------------"
	@echo "3. Тест: ./myls -l test_dir"
	@./myls -l test_dir
	@echo "------------------------------------------"
	@echo "4. Тест: ./myls -la test_dir"
	@./myls -la test_dir
	@echo "------------------------------------------"
	@echo "5. Тест: ./myls -l -a test_dir"
	@./myls -l -a test_dir
	@echo "------------------------------------------"
	@echo "6. Сравнение с оригинальным ls:"
	@echo "--- ls -la test_dir ---"
	@ls -la test_dir
	@echo "--- ./myls -la test_dir ---"
	@./myls -la test_dir
	@rm -rf test_dir

clean:
	rm -f myls