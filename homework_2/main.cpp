//
//  main.cpp
//  homework_2
//
//  Created by Furkan Ural on 24/11/2016.
//  Copyright © 2016 Furkan Ural. All rights reserved.
//

#include <iostream>
#include <math.h>
#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

class Pixel{
public:
    Pixel(int id, int b, int g, int r){
        this->id = id;
        this->b = b;
        this->g = g;
        this->r = r;
    }
    
    double distance(Mat img, int x, int y){
        Vec3b *v = new Vec3b(img.at<Vec3b>(x,y));
        double result = this->distance(v);
        free(v);
        return result;
    }
    
    double distance(Vec3b *v){
        //return ( abs((*v)[0] - this->b) + abs((*v)[1] - this->g) + abs((*v)[2] - this->r) );
        return sqrt( pow((*v)[0] - this->b , 2) +  pow((*v)[1] - this->g, 2) + pow((*v)[2] - this->r, 2) );
    }
    
    int getID(){
        return this->id;
    }
    int getB(){
        return b;
    }
    int getG(){
        return g;
    }
    int getR(){
        return r;
    }
    void setBGR(int b, int g, int r){
        if(b > 0 && b < 256)
            this->b = b;
        if(g > 0 && g < 256)
            this->g = g;
        if(r > 0 && r < 256)
            this->r = r;
    }
protected:
    int id, b, g, r;
};


class Image {
public:
    Image(string path){
        img = imread(path, CV_LOAD_IMAGE_COLOR);
    }
    
    Mat get_image(){
        return this->img;
    }
    
    bool write_image(string path){
        return imwrite(path, this->img);
    }
    
protected:
    Mat img;
};



class K_means {
public:
    K_means(Mat img, int cluster = 5, int epsilon = 0.5){
        this->cluster = cluster;
        this->img = img;
        label_img = new int*[img.rows];
        for(int i = 0; i < img.rows; ++i)
            label_img[i] = new int[img.cols];
        
        this->find_rand_centers();
        this->set_label_img();
        
        while (!this->find_new_centers(epsilon)) {
            this->set_label_img();
            cout << ".";
        }
        
    }
    
    int** get_labels(){
        return label_img;
    }
    
    Mat labels_as_image(){
        vector<Vec3b> colors(cluster);
        srand((unsigned)time(NULL));
        for(int i = 0; i < cluster; i++){
            colors[i] = Vec3b( (rand()&255), (rand()&255), (rand()&255) );
        }
        
        Mat result(img.size(), CV_8UC3);
        
        for (int i=0; i < img.rows; i++) {
            for (int j=0; j < img.cols; j++) {
                Vec3b &pixel = result.at<Vec3b>(i,j);
                pixel = colors[label_img[i][j]];
            }
        }
        
        return result;
    }
    
    vector<Pixel> get_centers(){
        return c_centers;
    }
    
protected:
    int cluster;
    Mat img;
    vector<Pixel> c_centers;
    int **label_img;
    
    bool find_new_centers(double epsilon){
        double theshold = 0;
        for (vector<Pixel>::iterator p = c_centers.begin(); p < c_centers.end(); ++p) {
            double sum_b = 0, sum_g = 0, sum_r = 0;
            int count = 0;
            for (int i = 0; i < img.rows; i++) {
                for (int j = 0; j < img.cols; j++) {
                    if (label_img[i][j] == (*p).getID()) {
                        Vec3b *v = new Vec3b(img.at<Vec3b>(i,j));
                        sum_b += (*v)[0];
                        sum_g += (*v)[1];
                        sum_r += (*v)[2];
                        count++;
                        free(v);
                    }
                }
            }
            int old_b = (*p).getB();
            int old_g = (*p).getG();
            int old_r = (*p).getR();
            (*p).setBGR( sum_b / count, sum_g / count,sum_r / count );
            theshold += abs((*p).getB() - old_b) + abs((*p).getG() - old_g) + abs((*p).getR() - old_r);
        }
        return (theshold <= epsilon);
    }
    
    void set_label_img() {
        for (int i=0; i < img.rows; i++) {
            for (int j = 0; j < img.cols ; j++) {
                double min_dist = c_centers.front().distance(img, i, j);
                int min_dist_id = c_centers.front().getID();
                for (Pixel p : c_centers) {
                    if( p.distance(img, i, j) < min_dist ){
                        min_dist = p.distance(img, i, j);
                        min_dist_id = p.getID();
                    }
                }
                label_img[i][j] = min_dist_id;
            }
        }
    }
    
    
    void find_rand_centers(){
        int x=0, y =0;
        srand((unsigned)time(NULL));
        for (int i = 0; i < cluster; i++) {
            x = rand() % img.rows;
            y = rand() % img.cols;
            Vec3b *v = new Vec3b(img.at<Vec3b>(x,y));
            Pixel p(i, (*v)[0], (*v)[1], (*v)[2]);
            c_centers.push_back( p );
        }
    }
};


int main(int argc, const char * argv[]) {
    /*
    string images[] = {"140075", "108005", "169012", "239007","253027", "124084", "155060", "176035", "25098","35010"};
    
    for (int k=5; k<16; k = k + 5) {
        cout << "running k means for " << to_string(k) << '\n';
        for (int i = 0; i < 10; i++) {
            cout << images[i] << " proccesing.";
            Image img("/images/" + images[i] + ".jpg");
            Mat image = img.get_image();
            K_means means(img.get_image(), k, 1);
            Mat labels = means.labels_as_image();
            imwrite("/images/" + images[i] + "_with_" + to_string(k) +".jpg", labels);
            cout << "done.\n";
        }
    }*/
    
    Image img("/Users/uralfrkn/Desktop/papagan.jpg");
    Mat image = img.get_image();
    K_means means(img.get_image(), 6);
    Mat labels = means.labels_as_image();
    imshow("result", labels);
    waitKey(0);
    //imwrite("/Users/uralfrkn/Desktop/new_papagan.jpg", labels);
    
    

    return 0;
}
