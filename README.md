# QuickNegativeConverter

## Description
---
A quick way to convert color and bw negatives into positive images.

- This is meant as a quick way to convert batches of negatives that you don't want to spend time converting individually.
	- This tool is not meant to replace better converters (like Negative Lab Pro)
- Color and contrast are usually acceptable (good enough for basic viewing)
### Limitations
- Does *not* work well on very dark images.
- Images need to already be cropped.


## Installation
---
Currently tested with Python 3.10.5.
Using pip to install requirements: <br>
```pip install -r requirements.txt```

## Usage
---
``` python NegativeQuickConvert.py [path] [negative_type] [-q QUALITY] [-t] ``` <br>

NQC has two required arguments: path and negative type. <br>
```[path]``` is the path to the folder with images <br>
```[negative_type]``` (bw/color) - sets either bw or color profile <br>

Optional arguments: <br>
```-t``` enables multicore processing (significantly speeds up processing)<br>
```-q <quality>``` Sets the jpeg compression quality (defaults to 50) <br>


Example usage:<br>
```python NegativeQuickConverter.py "/Desktop/Film/" bw -t -q 75 ```<br>
Convert images in the "Film" folder to BW jpgs with 75 quality, and use multicore processing.

All resulting images will be put inside a folder called "NegativeQuickConvert" where the images were located.

## License
---
MIT License
