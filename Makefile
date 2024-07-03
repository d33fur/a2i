A2I_DIR = a2i
CLI_DIR = cli
BIN_DIR = /usr/local/bin
SUDO := $(shell command -v sudo 2>/dev/null)
PV_INSTALLED := $(shell command -v pv 2>/dev/null)

GREEN := \033[0;32m
NC := \033[0m # No Color
WHITE := \033[1;37m

.PHONY: install-requirements build-a2i build-cli install-cli clean uninstall-requirements final-message

all: install-requirements build-a2i build-cli install-cli clean uninstall-requirements final-message

install-requirements:
ifndef !PV_INSTALLED
	@echo "Installing pv..." && \
	$(if $(SUDO),$(SUDO) apt-get update >/dev/null,  apt-get update >/dev/null) && \
	$(if $(SUDO),$(SUDO) apt-get install -y pv >/dev/null,  apt-get install -y pv >/dev/null)
endif

uninstall-requirements:
ifndef PV_INSTALLED
	@echo "Uninstalling pv..." && \
	$(if $(SUDO),$(SUDO) apt-get remove -y pv 2>&1 >/dev/null | pv -pt -e -r,  apt-get remove -y pv 2>&1 >/dev/null | pv -pt -e -r) && \
	$(if $(SUDO),$(SUDO) apt-get autoremove -y 2>&1 >/dev/null | pv -pt -e -r,  apt-get autoremove -y 2>&1 >/dev/null | pv -pt -e -r)
endif

build-a2i:
	@echo "Building A2I..." && \
	cd $(A2I_DIR) && \
	cmake -B build -S . > /dev/null && \
	$(if $(SUDO),$(SUDO) cmake --build build --target install 2>&1 >/dev/null | pv -pt -e -r,cmake --build build --target install 2>&1 >/dev/null | pv -pt -e -r) && \
	echo "$(GREEN)A2I build complete!$(NC)\n"

build-cli:
	@echo "Building CLI A2I..." && \
	cd $(CLI_DIR) && \
	cmake -B build -S . > /dev/null && \
	cmake --build build 2>&1 >/dev/null | pv -pt -e -r && \
	echo "$(GREEN)CLI build complete!$(NC)\n"

install-cli:
	@echo "Installing CLI A2I..." && \
	$(if $(SUDO),$(SUDO) cp $(CLI_DIR)/build/a2i $(BIN_DIR),cp $(CLI_DIR)/build/a2i $(BIN_DIR)) && \
	$(if $(SUDO),$(SUDO) chmod +x $(BIN_DIR)/a2i,chmod +x $(BIN_DIR)/a2i) && \
	echo "$(GREEN)CLI A2I installation complete!$(NC)\n"

clean:
	@echo "Cleaning up..." && \
	rm -rf $(A2I_DIR)/build $(CLI_DIR)/build && \
	$(MAKE) uninstall-requirements -s && \
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
