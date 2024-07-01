A2I_DIR = a2i
CLI_DIR = cli

.PHONY: all build-a2i build-cli install-cli clean

all: build-a2i build-cli install-cli clean

build-a2i:
	@cd $(A2I_DIR) && \
	cmake -B build -S . && \
	sudo cmake --build build --target install

build-cli:
	@cd $(CLI_DIR) && \
	cmake -B build -S . && \
	cmake --build build

install-cli:
	@sudo cp $(CLI_DIR)/build/a2i /usr/local/bin/ && \
	sudo chmod +x /usr/local/bin/a2i

clean:
	@rm -rf $(A2I_DIR)/build $(CLI_DIR)/build 
