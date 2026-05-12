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

#define M_PI           3.14159265358979323846
#include <math.h> //I wanted the accurate value of the PI constant.

//Firstly, I add in a structure I need later.
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

	int n_pixels_source = W * H;
	int n_pixels_target = W2 * H2;
	int n = std::min(n_pixels_source, n_pixels_target); //Even though there have been images provided of exactly equal size, I still try to use the minimum to be safe.

	// std::vector<double> image_double(W*H*3);
	// for (int i=0; i<W*H*3; i++)
	// 	image_double[i] = image[i];

	//Ofcourse convert to double for precise calculations, same thing just a bit easier for me to handle, I am doubling both.
	std::vector<double> I(n * 3);
    std::vector<double> M(n * 3);
    for (int i = 0; i < n * 3; i++) {
        I[i] = image_source[i];
        M[i] = image_target[i];
    }
	
	std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0); //Using the Real Uniform distribution from what I understood should be used...?

	int nbiter = 100; // Initial number of iterations to work with as recommended.
	std::cout << "[CHECK] Starting Sliced Optimal Transport for " << nbiter << " iterations..." << std::endl; //Intermediary Check print statement, so that I don't forget it later, I shall remove this later when everything works.
	
	//Main loop.
	for (int iter = 0; iter < nbiter; ++iter) {
		// Firstly, we generate the random direction on a spherre.
		double r1 = dist(gen);
		double r2 = dist(gen);

		//I try to use the exact formula from the lecture notes but I don't know if it would generalise (it is given for compensating for narmalized spheres.)
		double z = 1.0 - 2.0 * r2;
		double radius_xy = std::sqrt(std::max(0.0, 1.0 - z * z));
		Vec3 v = {
			std::cos(2.0 * M_PI * r1) * radius_xy,
			std::sin(2.0 * M_PI * r1) * radius_xy,
			z
		};

		//Now we project points ontp the direction (store the dot product and index).
		std::vector<std::pair<double, int>> projI(n);
		std::vector<std::pair<double, int>> projM(n);
		for (int i = 0; i < n; i++) {
			double dotI = I[i*3 + 0] * v.x + I[i*3 + 1] * v.y + I[i*3 + 2] * v.z;
			double dotM = M[i*3 + 0] * v.x + M[i*3 + 1] * v.y + M[i*3 + 2] * v.z;
			projI[i] = {dotI, i};
			projM[i] = {dotM, i};
		}
		//Now I sort according to dot product.
		std::sort(projI.begin(), projI.end());
		std::sort(projM.begin(), projM.end());

		//The important poart I guess, the advect initial point cloud implementation.
		for (int i = 0; i<n; i++) {
			int idxI = projI[i].second;
			int idxM = projM[i].second; //I am aware that we only need the value of projM[i].first, I would try to keep the struct simple/complete.
			
			double diff = projM[i].first - projI[i].first;
			I[(idxI*3) + 0] += diff * v.x;
			I[idxI*3 + 1] += diff * v.y;
			I[idxI * 3 + 2] += diff * v.z;
		}

		if ((iter + 1) % 10 == 0) {
            std::cout << "Iteration " << iter + 1 << "/" << nbiter << " completed." << std::endl; //Imp. Break or no over iter logic.
        }
	}

	// std::vector<unsigned char> image_result(W*H * 3, 0);
	// for (int i = 0; i < H; i++) {
	// 	for (int j = 0; j < W; j++) {

	// 		image_result[(i*W + j) * 3 + 0] = image_double[(i*W+j)*3+0]*0.5;
	// 		image_result[(i*W + j) * 3 + 1] = image_double[(i*W+j)*3+1]*0.3;
	// 		image_result[(i*W + j) * 3 + 2] = image_double[(i*W+j)*3+2]*0.2;
	// 	}
	// }

	// Converting back to image format, clamping to [0, 255], also I would bring back the original method once I revert back to the image_double method initially provided with the template.
    std::vector<unsigned char> image_result(W * H * 3, 0);
    for (int i = 0; i < n; i++) {
        image_result[i * 3 + 0] = static_cast<unsigned char>(std::clamp(I[i * 3 + 0], 0.0, 255.0));
        image_result[i * 3 + 1] = static_cast<unsigned char>(std::clamp(I[i * 3 + 1], 0.0, 255.0));
        image_result[i * 3 + 2] = static_cast<unsigned char>(std::clamp(I[i * 3 + 2], 0.0, 255.0));
    }

	// I am not sure if this needed but since I commented out the original implementation for now, I guess I would leave it in here (basically if source had more pixels than target we just copy the rest natively.)
    for (int i = n; i < W * H; i++) {
        image_result[i * 3 + 0] = image_source[i * 3 + 0];
        image_result[i * 3 + 1] = image_source[i * 3 + 1];
        image_result[i * 3 + 2] = image_source[i * 3 + 2];
    }

	// stbi_write_png("image.png", W, H, 3, &image_result[0], 0);
	//I am not using the same structure for now so I am not gonna use the pointer stuff here my Image result method is a bit different for me not to run into errors for now.
	stbi_write_png("output.png", W, H, 3, image_result.data(), 0);
    std::cout << "Saved successfully to output.png" << std::endl;
    stbi_image_free(image_source);
    stbi_image_free(image_target);

	return 0;
}