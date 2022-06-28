from PIL import Image
import numpy as np

def main():

    im = Image.open("font_example.bmp")
    (w, h) = im.size
    print(f"w: {w}, h: {h}")
    arr = np.array(im)
    pixels = []
    for col in range(h):
        for row in range(w):
            pixels.append(arr[col][row])

    print(len(pixels))

    combined = []
    for i in range(0, len(pixels), 8):
        val = 0
        for j in range(0, 8):
            is_lit = not bool(pixels[i+j])
            val = val | ( is_lit << (7-j))
        combined.append(val)

    ret = str()

    ret += "{"
    for val in combined:
        ret += "0x{:02x}".format(val)
        ret += ", "

    ret = ret[:-2]
    ret += "};"

    with open("out.txt", "w") as f:
        f.write(ret)



if __name__ == '__main__':
    main()
