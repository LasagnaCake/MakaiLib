	.global mkEmbed_MainShaderVert
    .global mkEmbed_MainShaderVertSize
    .section .rodata
mkEmbed_MainShaderVert:
    .incbin "shader.vert"
1:
mkEmbed_MainShaderVertSize:
    .int 1b - mkEmbed_MainShaderVert
