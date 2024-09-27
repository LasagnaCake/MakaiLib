	.global mkEmbed_MainShaderFrag
    .global mkEmbed_MainShaderFragSize
    .section .rodata
mkEmbed_MainShaderFrag:
    .incbin "shader.frag"
1:
mkEmbed_MainShaderFragSize:
    .int 1b - mkEmbed_MainShaderFrag
