A2I_DIR = a2i
CLI_DIR = cli
SUDO := $(shell command -v sudo 2>/dev/null)

.PHONY: all build-a2i build-cli install-cli clean

all: build-a2i build-cli install-cli clean

build-a2i:
	@cd $(A2I_DIR) && \
	cmake -B build -S . && \
	$(if $(SUDO),$(SUDO) cmake --build build --target install,cmake --build build --target install)

build-cli:
	@cd $(CLI_DIR) && \
	cmake -B build -S . && \
	cmake --build build

install-cli:
	@$(if $(SUDO),$(SUDO) cp $(CLI_DIR)/build/a2i /usr/local/bin/,cp $(CLI_DIR)/build/a2i /usr/local/bin/) && \
	$(if $(SUDO),$(SUDO) chmod +x /usr/local/bin/a2i,chmod +x /usr/local/bin/a2i)

clean:
	@rm -rf $(A2I_DIR)/build $(CLI_DIR)/build
