#=======================================================================
# Makefile for riscv-tests/isa
#-----------------------------------------------------------------------

act_dir := .
src_dir := $(act_dir)/src
work_dir := $(ROOTDIR)/work
work_dir_isa := $(work_dir)/$(RISCV_ISA)

include $(act_dir)/Makefrag
ifneq ($(RISCV_TEST),)
    target_tests = $(RISCV_TEST).elf
endif

default: all

#--------------------------------------------------------------------
# Build rules
#--------------------------------------------------------------------

vpath %.S $(act_dir)

INCLUDE=$(TARGETDIR)/$(RISCV_TARGET)/device/$(RISCV_DEVICE)/Makefile.include
ifeq ($(wildcard $(INCLUDE)),)
    $(error Cannot find '$(INCLUDE)`. Check that RISCV_TARGET and RISCV_DEVICE are set correctly.)
endif
-include $(INCLUDE)

#------------------------------------------------------------
# Build and run assembly tests

%.log: %.elf
	$(RUN_TARGET)


define compile_template

$$($(1)_tests): %.elf: $(src_dir)/%.S
	@mkdir -p $(work_dir_isa)/$$(@D)
	$(COMPILE_TARGET)
$(1)_tests += $$($(1)_p_tests)

.PHONY: $(1)

tests += $$($(1)_tests)

endef

$(eval $(call compile_template,rv32i,-march=rv32i -mabi=ilp32))

test_log = $(patsubst %.elf,%.log,$(target_tests))

run: $(test_log)

#------------------------------------------------------------
# Default

all: $(tests_dump)

#------------------------------------------------------------
# Clean up

clean:
	rm -rf $(work_dir)
