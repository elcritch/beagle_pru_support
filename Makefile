SUBDIRS = pru/lib/rpmsg pru/lib/pru_support pru/lib/msgpack pru/lib/softspic
TEST_SUBDIRS = test/softspi_test

export ARTIFACT_DIR = _build/pru

ifeq ($(MIX_ENV),test)
SUBDIRS += $(TEST_BUDIRS)
endif

all: artifact

artifact: $(SUBDIRS)

test: $(TEST_SUBDIRS)

_priv:
	mkdir -p priv/

artifact_build:
	mkdir -p $(ARTIFACT_DIR)/include
	cp -RL pru/include/* $(ARTIFACT_DIR)/include/

clean_artifact:
	rm -Rf $(ARTIFACT_DIR)/

$(SUBDIRS): artifact_build
	env | grep -i pru | sort
	$(MAKE) -C $@

$(TEST_SUBDIRS): artifact_build
	$(MAKE) -C $@

clean: clean_artifact
	$(eval export PRU_CGT := "/dev/null")
	@for d in $(SUBDIRS); do (cd $$d; $(MAKE) clean ); done
	@for d in $(TEST_SUBDIRS); do (cd $$d; $(MAKE) clean ); done

.PHONY: $(SUBDIRS)
