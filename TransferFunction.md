# Transfer Function

## Explain
* use volume value and gradient to show histogram and distribution
* render histogram and distribution for user setting transfer function
* user setting transfer function for slicing method

## Special
* user can select equalization or not
* both histogram and distribution can draw with mouse

## Dillemma
* transfer function isn't a class, hard to decide where it should be placed in my structure

## Member
```
MAX_GRADIENT_MAGNITUDE (float):
    max gradient magnitude

histogram (vector<float>):
    seperate volume value to 256 part, and calculate histogram

distribution (vector<vector<flot>>):
    seperate volume value to 256 part, set max gradient magnitude to MAX_GRADIENT_MAGNITUDE

red (vector<float>):
    red color for transfer function

green (vector<float>):
    green color for transfer function

blue (vector<float>):
    blue color for transfer function

alpha (vector<vector<float>>):
    alpha value for transfer function
```

## Member Function
```
load_volume:
    load volume for histogram and distribution
    - parameters:
        filename (string): volume filename
        histogram (vector<float>&): histogram
        distribution (vector<vector<float>>&): distribution
        equalization (bool): equalization or not

save_transfer_table:
    save rgba for tranfer table to transfer_function.txt
    - parameters:
        filename (string): volume filename
        color (vector<vector<float>>&): shpae (3, 256), combine red, green, blue
        alpha (vector<vector<float>>&): alpha table
```