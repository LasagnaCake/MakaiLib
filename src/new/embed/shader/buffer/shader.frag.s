	.global mkEmbed_BufferShaderFrag
    .global mkEmbed_BufferShaderFragSize
    .section .rodata
mkEmbed_BufferShaderFrag:
    .incbin "shader.frag"
1:
mkEmbed_BufferShaderFragSize:
    .int 1b - mkEmbed_BufferShaderFrag
