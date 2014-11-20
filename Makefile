.PHONY: clean All

All:
	@echo "----------Building project:[ ktcl - Release ]----------"
	@$(MAKE) -f  "ktcl.mk"
clean:
	@echo "----------Cleaning project:[ ktcl - Release ]----------"
	@$(MAKE) -f  "ktcl.mk" clean
