idf_component_register(SRCS "blink_example_main.c"
                    INCLUDE_DIRS "."
					PRIV_REQUIRES 
					freertos
					driver
					led_strip
					)
