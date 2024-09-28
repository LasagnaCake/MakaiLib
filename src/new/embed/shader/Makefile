#embed = @ld -r -b binary -o $(prefix).$@.$(strip $(1)).o **/*.vert **/*.frag

LEAN = $(if $(findstring debug,$@),,-static -s)

define embed
	@cd main
	@gcc -o main.vert.shader.o -c shader.vert.s $(LEAN)
	@gcc -o main.frag.shader.o -c shader.frag.s $(LEAN)
	@cd ..
	@cd buffer
	@gcc -o buffer.vert.shader.o -c shader.vert.s $(LEAN)
	@gcc -o buffer.frag.shader.o -c shader.frag.s $(LEAN)
	@cd ..
	@ld -r -o $(prefix).$@.$(strip $(1)).o **/*.shader.o $(LEAN)
endef

.ONESHELL:
.SHELLFLAGS = -ec

shader:
	$(call embed, debug)
	$(call embed, release)