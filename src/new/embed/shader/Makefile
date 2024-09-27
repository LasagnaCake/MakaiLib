#embed = @ld -r -b binary -o $(prefix).$@.$(strip $(1)).o **/*.vert **/*.frag
define embed
	@cd main
	@gcc -o main.vert.shader.o -c shader.vert.s
	@gcc -o main.frag.shader.o -c shader.frag.s
	@cd ..
	@cd buffer
	@gcc -o buffer.vert.shader.o -c shader.vert.s
	@gcc -o buffer.frag.shader.o -c shader.frag.s
	@cd ..
	@ld -r -o $(prefix).$@.$(strip $(1)).o **/*.shader.o
endef

.ONESHELL:
.SHELLFLAGS = -ec

shader:
	$(call embed, debug)
	$(call embed, release)