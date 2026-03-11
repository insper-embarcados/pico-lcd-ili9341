add_library(gfx
	#gfx_adafruit.c
	gfx_ili9341.c
)

target_include_directories(gfx PUBLIC
	.
)

target_link_libraries(gfx pico_stdlib hardware_dma)