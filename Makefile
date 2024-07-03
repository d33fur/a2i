A2I_DIR = a2i
CLI_DIR = cli
BIN_DIR = /usr/local/bin
SUDO := $(shell command -v sudo 2>/dev/null)

GREEN := \033[0;32m
NC := \033[0m

.PHONY: build-a2i build-cli install-cli clean final-message

all: build-a2i build-cli install-cli clean final-message

build-a2i:
	@echo "Building A2I..." && \
	cd $(A2I_DIR) && \
	cmake -B build -S . > /dev/null && \
	$(if $(SUDO),$(SUDO) cmake --build build --target install 2>&1 >/dev/null,cmake --build build --target install 2>&1 >/dev/null) && \
	echo "$(GREEN)A2I build complete!$(NC)\n"

build-cli:
	@echo "Building CLI A2I..." && \
	cd $(CLI_DIR) && \
	cmake -B build -S . > /dev/null && \
	cmake --build build 2>&1 >/dev/null && \
	echo "$(GREEN)CLI build complete!$(NC)\n"

install-cli:
	@echo "Installing CLI A2I..." && \
	$(if $(SUDO),$(SUDO) cp $(CLI_DIR)/build/a2i $(BIN_DIR),cp $(CLI_DIR)/build/a2i $(BIN_DIR)) && \
	$(if $(SUDO),$(SUDO) chmod +x $(BIN_DIR)/a2i,chmod +x $(BIN_DIR)/a2i) && \
	echo "$(GREEN)CLI A2I installation complete!$(NC)\n"

clean:
	@echo "Cleaning up..." && \
	rm -rf $(A2I_DIR)/build $(CLI_DIR)/build && \
	echo "$(GREEN)Clean up complete!$(NC)\n"

final-message:
	@printf '_____/\\\\\\\\\\\\\\\\\\_______/\\\\\\\\\\\\\\\\\\______/\\\\\\\\\\\\\\\\\\\\\\_        \n'
	@printf ' ___/\\\\\\\\\\\\\\\\\\\\\\\\\\___/\\\\\\///////\\\\\\___\\/////\\\\\\///__       \n'
	@printf '  __/\\\\\\/////////\\\\\\_\\///______\\//\\\\\\______\/\\\\\\_____      \n'
	@printf '   _\\/\\\\\\_______\/\\\\\\___________/\\\\\\/_______\/\\\\\\_____     \n'
	@printf '    _\\/\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\________/\\\\\\//_________\/\\\\\\_____    \n'
	@printf '     _\\/\\\\\\/////////\\\\\\_____/\\\\\\//____________\/\\\\\\_____   \n'
	@printf '      _\\/\\\\\\_______\\/\\\\\\___/\\\\\\/_______________\/\\\\\\_____  \n'
	@printf '       _\\/\\\\\\_______\\/\\\\\\__/\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\__/\\\\\\\\\\\\\\\\\\\\\\_ \n'
	@printf '        _\\///________\\///__\///////////////__\///////////__\n\n'
	@echo "$(GREEN)Thank you for using our software!$(NC)"
	@echo "Please visit the following links for more information:"
	@echo "Documentation: $(GREEN)https://d33fur.github.io/a2i/index.html$(NC)"
	@echo "GitHub: $(GREEN)https://github.com/d33fur/a2i$(NC)"
	@echo "For more information, run: $(GREEN)a2i --help$(NC)"
