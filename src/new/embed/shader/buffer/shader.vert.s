	.global mkEmbed_BufferShaderVert
    .global mkEmbed_BufferShaderVertSize
    .section .rodata
mkEmbed_BufferShaderVert:
    .incbin "shader.vert"
1:
mkEmbed_BufferShaderVertSize:
    .int 1b - mkEmbed_BufferShaderVert
