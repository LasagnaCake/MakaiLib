#embed = @ld -r -b binary -o $(prefix).$@.$(strip $(1)).o **/*.vert **/*.frag

LEAN = $(if $(findstring debug,$@),,)#-static -s)

define embed
	@cd main
	@$(CC) -o main.vert.shader.o -c shader.vert.s $(LEAN)
	@$(CC) -o main.frag.shader.o -c shader.frag.s $(LEAN)
	@cd ..
	@cd buffer
	@$(CC) -o buffer.vert.shader.o -c shader.vert.s $(LEAN)
	@$(CC) -o buffer.frag.shader.o -c shader.frag.s $(LEAN)
	@cd ..
	@$(LD) -r -o $(prefix).$@.$(strip $(1)).o **/*.shader.o $(LEAN)
endef

.ONESHELL:
.SHELLFLAGS = -ec

shader:
	$(call embed, debug)
	$(call embed, release)