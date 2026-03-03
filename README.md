# Jigsaw Puzzle Jgraph

This program uses jgraph to generate a jigsaw puzzle outline on an input image.

## Requirements
This repository assumes the following are installed:
- `jgraph`
- `ps2pdf`
- `convert`


## How to Run

### Automatically compile and run to create 5 example puzzles
1. Clone this repository.
2. Run `make all` 


This will:
- Compile `puzzle.cpp` 
- Create the `jgraphs/` and `results/` folders
- Generate jgraph files for each image (located in `jgraphs/`)
- Generate the final puzzle images for each image (located in `results/`) <br>
Note: Each puzzle may take 1-2 minutes to run due to image size.


### Manually compile and run program

1. Convert an image to EPS format
```bash
convert imagefile image.eps
```

Optional: If the image is very large the program will take a long time to run, you might need to resize first.
```bash
convert imagefile -resize 50% newimagefile
```

2. Compile the program
```bash
g++ -o puzzle puzzle.cpp
```

3. Run program to create jgraph file
`puzzle.cpp` requires three command line arguments:
- EPS image file
- Number of puzzle rows
- Number of puzzle columns
```bash
./puzzle image.eps rows cols > jgraph_file.jgr
```

4. Create final puzzle image using jgraph <br>
You'll need to manually crop the final puzzle image.
```bash
jgraph -P jgraph_file.jgr | ps2pdf - | convert -density 300 - -quality 100 final_puzzle.jpg
```


## Program Description

`puzzle.cpp` creates a jigsaw puzzle layout on an image by:
1. Reading in the EPS image file and puzzle dimensions
2. Randomly assigning each puzzle piece edge a shape: tab (protruding part) or blank (intruding part)
3. Generating puzzle piece outlines using Bezier curves
4. Outputting the corresponding jgraph file information

### Defaults
I've set default values for certain variables in my program, these can be updated in the future based on preferred puzzle style:
- Tab size: ~30% of the piece edge length
- Tab position: center of each edge
- Outline style: Black, 1.5 line thicknesss


### Example: Lighthouse Illustration

1. Started with `images/lighthouse.png` <br>
<img src=images/lighthouse.png width="50%" />

2. Converted to `eps_files/lighthouse.eps`

3. Generated puzzle with `make lighthouse`

4. Final puzzle image saved to `results/lighthouse_puzzle.jpg` <br>
Note: I manually cropped excess whitespace <br>
<img src=readme_example.jpg width="50%" />

<br>
Additional Note: I found all of my images on https://pixabay.com


