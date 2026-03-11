add_executable(touch_resistive
        touch_resistive.c
        )

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(touch_resistive pico_stdlib ili9341 gfx hardware_adc)

# create map/bin/hex file etc.
pico_add_extra_outputs(touch_resistive)

