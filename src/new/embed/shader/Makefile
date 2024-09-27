embed = @ld -r -b binary -o $(prefix).$@.$(strip $(1)).o **/*.* --no-leading-underscore

shader:
	$(call embed, debug)
	$(call embed, release)