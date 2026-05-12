#define _CRT_SECURE_NO_WARNINGS 1
#include <vector>
//Add in the necessary imports for this lab.
#include <iostream>
#include <algorithm> //I might not use it I am just adding it in for the sake of it.
#include <cmath>
#include <random>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//Firstly, I add in a structure for a 3D vector sicne the vector class might not be needed here, this shall suffice.
struct Vec3 {
	double x, y, z;
};

int main() {

	int W, H, C;
	int W2, H2, C2;
	
	//stbi_set_flip_vertically_on_load(true);
	// unsigned char *image = stbi_load("8733654151_b9422bb2ec_k.jpg",
    //                              &W,
    //                              &H,
    //                              &C,
    //                              STBI_rgb);
	
	// I just slightly tweaked the given method to keep the naming more easy for me to work with...
    unsigned char *image_source = stbi_load("source.jpg", &W, &H, &C, STBI_rgb);
    if (!image_source) {
        std::cerr << "Failed to load source image! Exiting." << std::endl; //In case my image is named wrong.
        return -1;
    }

	//Now about loading the target (model) image I write a similar method just for convenience.
    unsigned char *image_target = stbi_load("target.jpg", &W2, &H2, &C2, STBI_rgb);
    if (!image_target) {
        std::cerr << "Failed to load target image! Exiting." << std::endl;
        stbi_image_free(image_source);
        return -1;
    }

	std::vector<double> image_double(W*H*3);
	for (int i=0; i<W*H*3; i++)
		image_double[i] = image[i];
	
	std::vector<unsigned char> image_result(W*H * 3, 0);
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {

			image_result[(i*W + j) * 3 + 0] = image_double[(i*W+j)*3+0]*0.5;
			image_result[(i*W + j) * 3 + 1] = image_double[(i*W+j)*3+1]*0.3;
			image_result[(i*W + j) * 3 + 2] = image_double[(i*W+j)*3+2]*0.2;
		}
	}
	stbi_write_png("image.png", W, H, 3, &image_result[0], 0);

	return 0;
}