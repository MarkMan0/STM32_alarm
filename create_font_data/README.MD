# This directory contains examples on how to create a font to be used with the GFX library

Only monospace fonts are supported
1. Install the font into the system

1. Create a text file of the characters you would like to render (create_txt_file.py)

1. Create a canvas in gimp, the height of the font and the width of the font times the number of characters(font_example.xcf), in this case 8 x 720. The background should be white, and the font black, without anti-aliasing

1. copy the text from the generated file onto the canvas in gimp, with the required font

1. export the image as bmp

1. feed the image into img_to_code.py, and get the array of bytes

1. copy contents of out.txt into code
