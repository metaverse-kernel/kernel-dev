SOURCE := $(shell pwd)/src/scripts

.PHONY: all clear run debug config

all: config
	@make -C src all --no-print-directory

clear: config
	@make -C src clear --no-print-directory

run: config
	@make -C test run --no-print-directory

debug: config
	@make -C test debug --no-print-directory

config:
	@if [ "${shell uname -s}" != "Linux" ]; then \
		echo -e "\e[1;33mMetaverse\e[0m must build under \e[1;35mLinux\e[0m or itself."; \
		exit -1; \
	fi
	@if [ -f "metaverse_kernel" ]; then \
		echo; \
	else \
		touch metaverse_kernel; \
		"${SOURCE}/depcheck"; \
	fi
