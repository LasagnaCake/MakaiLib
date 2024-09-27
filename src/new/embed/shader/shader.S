	.global main_shader_vert
	.global main_shader_vert_size
	.global main_shader_frag
	.global main_shader_frag_size
	.global buffer_shader_vert
	.global buffer_shader_vert_size
	.global buffer_shader_frag
	.global buffer_shader_frag_size
    .section .rodata
main_shader_vert:
    .incbin "main/shader.vert"
1:
main_shader_frag:
    .incbin "main/shader.frag"
2:
buffer_shader_vert:
    .incbin "buffer/shader.vert"
3:
buffer_shader_frag:
    .incbin "buffer/shader.frag"
4:
main_shader_vert_size:
    .int 1b - main_shader_vert
main_shader_frag_size:
    .int 2b - main_shader_frag
buffer_shader_vert_size:
    .int 3b - buffer_shader_vert
buffer_shader_frag_size:
    .int 4b - buffer_shader_frag
