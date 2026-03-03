/*
 * Lab1 -- Jgraph
 * Name -- Fatima Bowers
 * Due -- 03/02/26 at 2 am
 * 
 * Description -- Generates a jgraph file of a jigsaw puzzle. 
 * Takes input of an image as an eps file and the number of rows and columns of the puzzle.
 * Prints the jgraph file information as standard output.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <random>

using namespace std;

struct Edge {
    double x, y;                // starting position of the edge
    double length;              // total length of edge

    int tab_direction;          // type of edge: 1 for tab (protruding part), -1 for blank (intruding part)
    double tab_scale = 0.3;     // how to scale the tab size
    double tab_location = 0.5;  // where on the edge the tab is located
    string bezier_points;       // string of all points used to create the bezier curve of the edge
};

class PuzzleBoard {
    public:
        string image_eps;                   // eps file name of the image, argv[1]
        int rows, cols;                     // number of rows and cols of the puzzle, argv[2] argv[3]          
        void createJgraph();      

    private:
        double image_width, image_height;   // dimensions of the image
        double piece_width, piece_height;   // dimensions of the puzzle pieces (scaled to jgraph)

        vector<Edge> horizontal_edges;      // all horizontal edges between two puzzle pieces
        vector<Edge> vertical_edges;        // all vertical edges between two puzzle pieces

        void getDimensions();     
        void generateEdges();
        void generateBezierPoints(Edge &edge, bool horizontal);
};

int main(int argc, char **argv)
{
    PuzzleBoard board;

    if (argc != 4) {
        cerr << "usage: ./program image_file puzzle_rows puzzle_cols\n";
        exit(1);
    }

    board.image_eps = argv[1];
    board.rows = stoi(argv[2]);
    board.cols = stoi(argv[3]);

    board.createJgraph();
    return 0;
}

// helper function for scaling down the image size for jgraph
int find_gcd(int a, int b) {
    if (b == 0) return a;
    return find_gcd(b, a % b);
}



/* All Class/Struct Methods */

/* 
 * creates the jgraph, printed as standard output
 * graphs the outline of the jigsaw puzzle/pieces
 */
void PuzzleBoard::createJgraph()
{
    double common_factor, scaled_width, scaled_height, axis_length;

    // gets the image width and height from the eps file
    getDimensions();

    // scale down dimensions for the jgraph
    common_factor = find_gcd(image_width, image_height);
    scaled_width = image_width / common_factor;
    scaled_height = image_height / common_factor; 
    piece_width =  scaled_width / cols;
    piece_height = scaled_height / rows;

    // the x and y axes of the jgraph will be the size of the larger side of the image
    // necessary to keep image proportions in jgraph
    axis_length = max(scaled_width, scaled_height);

    // create the graph in jgraph and add image as background
    printf("newgraph\n");
    printf("xaxis min 0 max %lf nodraw\n", axis_length);
    printf("yaxis min 0 max %lf nodraw\n", axis_length);
    printf("newcurve eps %s marksize %lf %lf pts %lf %lf\n", image_eps.c_str(), scaled_width, scaled_height, scaled_width / 2, scaled_height / 2);

    // outline the puzzle, this creates the straight edges of the outer puzzle pieces
    printf("newline linethickness 1.5 color 0 0 0 pts\n");
    printf("0 0 0 %lf\n", scaled_height);
    printf("0 %lf %lf %lf\n", scaled_height, scaled_width, scaled_height);
    printf("%lf %lf %lf 0\n", scaled_width, scaled_height, scaled_width);
    printf("%lf 0 0 0\n", scaled_width);

    // create the vertical and horizontals edges between the puzzle pieces
    generateEdges();

    // add the horizontal edges to the graph
    for (int i = 0; i < (int)horizontal_edges.size(); i++) {
        Edge edge = horizontal_edges[i];    
        printf("newline linethickness 1.5 color 0 0 0 bezier pts\n");
        printf("%s\n", edge.bezier_points.c_str());
    }

    // add the vertical edges to the graph
    for (int i = 0; i < (int)vertical_edges.size(); i++) {
        Edge edge = vertical_edges[i];    
        printf("newline linethickness 1.5 color 0 0 0 bezier pts\n");
        printf("%s\n", edge.bezier_points.c_str());
    }
}


/*
 * gets the image width and height from the eps file header details
 */
void PuzzleBoard::getDimensions()
{
    ifstream fin(image_eps);
    string line, temp;
    double llx = 0, lly = 0; // boundary box: lower left x, lower left y
    double urx = 0, ury = 0; // boundary box: upper right x, upper right y

    if (!fin) {
        cerr << "Could not open eps file\n";
        exit(1);
    }

    // read the lines of the eps file header, look for the BoundingBox line
    while (getline(fin, line)) {
        if (line.find("%%BoundingBox:") != string::npos) {
            // save the bounding box details
            // line format -- %%BoundingBox: llx lly urx urv

            istringstream iss(line);
            iss >> temp >> llx >> lly >> urx >> ury;
            break;
        }

        // reached end of header
        if (line.find("%%EndComments") != string::npos) break;
    }

    // calculate dimensions
    image_width = urx - llx;
    image_height = ury - lly;

    if (image_width == 0 && image_height == 0) {
        cerr << "Could not find BoundingBox in eps file\n";
        exit(1);
    }
}

/*
 * creates the horizontal and vertical edges of the puzzle, excluding straight edges (puzzle border)
 * each edge is between two pieces
 */
void PuzzleBoard::generateEdges()
{
    int r, c;

    random_device rd;
    mt19937 gen(15);  // mt19937 gen(rd()); -- for random seed

    // used to randomly decide if the edge is a tab or a blank  
    uniform_int_distribution<> dist_shape(-1, 1);

    // horizontal edges, only counts the edges between two pieces
    for (r = 1; r < rows; r++) {
        for (c = 0; c < cols; c++) {
            Edge edge;

            // set starting position and length of edge
            edge.x = c * piece_width;
            edge.y = r * piece_height;
            edge.length = piece_width;

            // randomly decide if tab or blank
            edge.tab_direction = (dist_shape(gen) >= 0) ? 1 : -1;

            // creates the points needed to draw the bezier curve of the edge
            generateBezierPoints(edge, true);

            horizontal_edges.push_back(edge);
        }
    }

    // vertical edges, only counts the edges between two pieces
    for ( c = 1; c < cols; c++) {
        for (r = 0; r < rows; r++) {
            Edge edge;

            // set starting position and length of edge
            edge.x = c * piece_width;
            edge.y = r * piece_height;            
            edge.length = piece_height;

            // randomly decide if tab or blank
            edge.tab_direction = (dist_shape(gen) >= 0) ? 1 : -1;

            // creates the points needed to draw the bezier curve of the edge
            generateBezierPoints(edge, false);

            vertical_edges.push_back(edge);
        }
    }
}


/* 
 * calculates the points for the bezier curve -- stored as a string in the edge struct
 * creates the edge of the piece as a bezier curve
 * the edge contains 3 curves, so 3n + 1 bezier points = 10 total points
 * 1 curve for left side of neck
 * 1 curve for protruding/intruding part of the tab
 * 1 curve for right side of neck
 */
void PuzzleBoard::generateBezierPoints(Edge &edge, bool horizontal)
{
    ostringstream oss;  // used to create the string holding all points
    double tab_center;  // location of the middle of the tab, can change depending on edge.tab_location
    double tab_depth;   // the scaled length of the tab based on the average of the piece width and height
    double half_tab;    // half of the tab length 
    double half_neck;   // fourth of the tab length

    tab_center = edge.tab_location * edge.length;
    tab_depth = edge.tab_scale * ((piece_width + piece_height) / 2);
    half_tab = tab_depth / 2;
    half_neck = tab_depth / 4;

    // P1 -- starting point of the edge
    oss << edge.x << " " << edge.y << " ";

    // P2 -- (left) shoulder point of the edge, between start and neck
    if (horizontal) oss << (edge.x + tab_center - half_neck) << " " << (edge.y - edge.tab_direction * half_neck) << " ";
    else oss << (edge.x - edge.tab_direction * half_neck) << " " << (edge.y + tab_center - half_neck) << " ";

    // P3 -- (left) neck point of the edge, between shoulder and start of tab
    if (horizontal) oss << (edge.x + tab_center - half_neck) << " " << (edge.y - edge.tab_direction * half_neck) << " ";
    else oss << (edge.x - edge.tab_direction * half_neck) << " " << (edge.y + tab_center - half_neck) << " ";

    // P4 -- (left) starting point of the tab, between neck and end of tab
    if (horizontal) oss << (edge.x + tab_center - half_tab) << " " << (edge.y + edge.tab_direction * half_neck / 2) << " ";
    else oss << (edge.x + edge.tab_direction * half_neck / 2) << " " << (edge.y + tab_center - half_tab) << " ";

    // P5 -- (left) ending point of the tab
    if (horizontal) oss << (edge.x + tab_center - tab_depth) << " " << (edge.y + edge.tab_direction * tab_depth) << " ";
    else oss << (edge.x + edge.tab_direction * tab_depth) << " " << (edge.y + tab_center - tab_depth) << " ";

    // P6 -- (right) ending point of the tab
    if (horizontal) oss << (edge.x + tab_center + tab_depth) << " " << (edge.y + edge.tab_direction * tab_depth) << " ";
    else oss << (edge.x + edge.tab_direction * tab_depth) << " " << (edge.y + tab_center + tab_depth) << " ";

    // P7 -- (right) starting point of the tab, between neck and end of tab
    if (horizontal) oss << (edge.x + tab_center + half_tab) << " " << (edge.y + edge.tab_direction * half_neck / 2) << " ";
    else oss << (edge.x + edge.tab_direction * half_neck / 2) << " " << (edge.y + tab_center + half_tab) << " ";

    // P8 -- (right) neck point of the edge, between shoulder and start of tab
    if (horizontal) oss << (edge.x + tab_center + half_neck) << " " << (edge.y - edge.tab_direction * half_neck) << " ";
    else oss << (edge.x - edge.tab_direction * half_neck) << " " << (edge.y + tab_center + half_neck) << " ";

    // P9 -- (right) shoulder point of the edge, between start and neck
    if (horizontal) oss << (edge.x + tab_center + half_neck) << " " << (edge.y - edge.tab_direction * half_neck) << " ";
    else oss << (edge.x - edge.tab_direction * half_neck) << " " << (edge.y + tab_center + half_neck) << " ";

    // P10 -- ending point of the edge
    if (horizontal) oss << (edge.x + edge.length) << " " << edge.y << " ";
    else oss << edge.x << " " << (edge.y + edge.length) << " ";

    // store string of points
    edge.bezier_points = oss.str();
}
