# Example code

Write value in line 1, line 2 of LCD

```py
import lcd_driver
import time

display = lcd_driver.lcd()

# Main body of code
try:
    while True:
        display.lcd_display_string("Hello, World!", 1) # Write line of text to first line of display
        display.lcd_display_string("Value in line 2", 2) # Write line of text to second line of display

except KeyboardInterrupt: # If there is a KeyboardInterrupt (when you press ctrl+c), exit the program and cleanup
    print("Cleaning up!")
    display.lcd_clear()
```    
