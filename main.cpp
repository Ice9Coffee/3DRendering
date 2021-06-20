#include "raytracer.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <cstdio>

Scene* Scene0();    //test
Scene* Scene1();    //Cornell box
Scene* Scene2();    //6 balls with 4 area lights (very slow)
Scene* Scene3();    //6 balls with 1 point light
Scene* Scene4();    //12 balls (more beautiful with fish eye sampler)
Scene* Scene5();    //bezier
Texture* readTex(char* filename) {
    cv::Mat mat = cv::imread(filename);
    Texture* tex = new Texture();
    tex->h_pix = mat.cols;
    tex->w_pix = mat.rows;
    for(int i = 0; i<mat.rows; ++i)
        for(int j = 0; j < mat.cols; ++j) {
            cv::Vec3b cl = mat.at<cv::Vec3b>(i, j);
            tex->map.push_back(Color(cl[2], cl[1], cl[0]) / 255);
        }
    return tex;
}

int main() {

    std::string savename = "out/img_s3.png";

    Scene* scene = Scene3();

    //scene->camera.set(Vector3(0.0, 2.6, -4.5), Vector3(0.0, 1.0, 0.0), Vector3(0.0, -4, 4.5));
    Sampler* sp = new Sampler(300, 300, 1.5, 1, 1);

    //scene->camera.set(Vector3(0.0, 9, 0.0), Vector3(0.0, 0.0, 1.0), Vector3(0.0, -1.0, 0.0));
    //Sampler* sp = new FishEyeSampler(2048, 2048, 1.3, 2, 2);

    //scene->camera.set(Vector3(0.0, 0.6, 0.0), Vector3(0.0, 1.0, 0.0), Vector3(1.0, 0.0, 0.0));
    //Sampler* sp = new WideAngleSampler(6000, 1000, PI, 2, 2);
    scene->camera.M.print();

    RayTracer RT;
    RT.setAmbient(0.1);
    RT.setMaxDepth(10);
    RT.setLightQuality(1);

    Film* film1 = RT.Run(scene, sp);

    cv::Mat img(film1->H(), film1->W(), CV_32FC3);

    for(int i=0; i<film1->H();++i)
        for(int j = 0; j < film1->W(); ++j) {
            Color c = film1->at(i, j).confine();
            cv::Vec3f color(c.b, c.g, c.r);
            img.at<cv::Vec3f>(i, j) = color;
        }

    cv::imshow("img", img);
    img *= 255.0;
    cv::imwrite(savename, img);

    printf("completed!\n");
    printf("inverse matrix calculated = %d\n", Transform::inverse_calcued);

    cv::waitKey(0);
    return 0;
}

Scene* Scene0() {
    Scene* scene0 = new Scene();

    scene0->camera.set(Transform::Scale(1.0).translate_(Vector3(0.0, 2.0, 0.0)));

    Material* mat = new Material(*Material::DEFAULT);
    scene0->prim.push_back(new Plane(Vector3(0.0, 0.0, 0.0), Vector3(1.0, 0.0, 0.0), Vector3(0.0, 0.0, -1.0), mat));

    scene0->light.push_back(new PointLight(50, Color::white(), Vector3(0.0, 4.0, 8.0)));
    
    return scene0;
}

Scene* Scene1() {
    Scene* scene1 = new Scene();

    scene1->camera.set(
        Vector3(0.0, 2.0, -2.0),
        Vector3(0.0, 1.0, 0.0),
        Vector3(0.0, -0.5, 1.0)
    );
    scene1->camera.set(Transform::Scale(1.0).translate_(Vector3(0.0, 2.0, -2.0)));

    Material* ball_mat = new Material(Color::white(), 0.0, 0.0, 10, 0.2, Color(0.3, 0.3, 0.3), 0.9, 1.2);
    Material* ball1_mat = new Material(Color::yellow(), 0.8, 0.2, 8, 0.2, Color(1.0, 1.0, 0.0), 0.0, 1.0);
    Material* ball2_mat = new Material(Color::red(), 0.9, 0.3, 5, 0.1, Color(0.0, 0.0, 1.0), 0.0, 1.0);
    Material* floor_mat = new Material(*Material::DEFAULT);
    Material* lwall_mat = new Material(*Material::DEFAULT);
    Material* rwall_mat = new Material(*Material::DEFAULT);
    Material* other_mat = new Material(*Material::DEFAULT);
    floor_mat->color = Color::white() * 0.5;
    lwall_mat->color = Color::green();
    rwall_mat->color = Color::cyan();
    other_mat->color = Color::white() * 0.8;

    //floor_mat->tex = readTex("tex/wood.bmp");


    Plane* floor = new Plane(
        Vector3(0.0, 0.0, 0.0),
        Vector3(4.0, 0.0, 0.0),
        Vector3(0.0, 0.0, -4.0),
        floor_mat
    );
    Plane* ceiling = new Plane(
        Vector3(0.0, 4.05, 0.0),
        Vector3(1.0, 0.0, 0.0),
        Vector3(0.0, 0.0, 1.0),
        other_mat
    );
    Plane* lwall = new Plane(
        Vector3(-2.00, 0.0, 0.0),
        Vector3(0.0, 1.0, 0.0),
        Vector3(0.0, 0.0, 1.0),
        lwall_mat
    );
    Plane* rwall = new Plane(
        Vector3(2.00, 0.0, 0.0),
        Vector3(0.0, 0.0, 1.0),
        Vector3(0.0, 1.0, 0.0),
        rwall_mat
    );
    Plane* bwall = new Plane(
        Vector3(0.0, 0.0, 3.00),
        Vector3(1.0, 0.0, 0.0),
        Vector3(0.0, -1.0, 0.0),
        other_mat
    );
    Plane* fwall = new Plane(
        Vector3(0.0, 0.0, -2.00),
        Vector3(1.0, 0.0, 0.0),
        Vector3(0.0, 1.0, 0.0),
        other_mat
    );
    Sphere* ball = new Sphere(
        Vector3(1.2, 2.0, 0.6),
        0.6, 0.0, 0.0,
        ball_mat
    );
    Sphere* ball1 = new Sphere(
        Vector3(-1.5, 0.4, 1.6),
        0.4, 0.0, 0.0,
        ball1_mat
    );
    Sphere* ball2 = new Sphere(
        Vector3(-0.5, 0.2, 0.5),
        0.2, 0.0, 0.0,
        ball2_mat
    );


    AreaLight* a_light0 = new AreaLight(
        8, Color::white(),
        Transform::Scale(1.0).translate_(Vector3(0.0, 4.00, 0.0))
    );

    PointLight* p_light0 = new PointLight(
        4, Color::white(), Vector3(0.0, 2.0, 1.0)
    );
    PointLight* p_light1 = new PointLight(
        1, Color::white(), Vector3(0.0, 0.2, -2.0)
    );
    PointLight* p_light2 = new PointLight(
        0.5, Color::white(), Vector3(-1.8, 3.8, 1.0)
    );
    PointLight* p_light3 = new PointLight(
        0.5, Color::white(), Vector3( 1.8, 3.8, 1.0)
    );

    scene1->prim.push_back(floor);
    scene1->prim.push_back(ceiling);
    scene1->prim.push_back(lwall);
    scene1->prim.push_back(rwall);
    scene1->prim.push_back(bwall);
    scene1->prim.push_back(fwall);
    scene1->prim.push_back(ball);
    scene1->prim.push_back(ball1);
    scene1->prim.push_back(ball2);
    scene1->light.push_back(a_light0);
    //scene1->light.push_back(p_light0);
    //scene1->light.push_back(p_light1);
    //scene1->light.push_back(p_light2);
    //scene1->light.push_back(p_light3);

    return scene1;
}

Scene* Scene2() {
    Scene* scene2 = new Scene();

    scene2->camera.set(
        Vector3(0.0, 4.0, -3.0),
        Vector3(0.0, 1.0, 0.0),
        Vector3(0.0, -0.72, 1.0)
    );

    Material* ball1_mat = new Material(Color::white(), 0.0, 0.0, 10, 0.6, Color(0.0, 1.0, 1.0), 0.9, 1.4);
    Material* ball2_mat = new Material(Color::white(), 0.0, 0.0, 10, 0.6, Color(1.0, 0.0, 1.0), 0.9, 1.4);
    Material* ball3_mat = new Material(Color::white(), 0.0, 0.0, 10, 0.6, Color(1.0, 1.0, 0.0), 0.9, 1.4);
    Material* ball4_mat = new Material(Color::white(), 0.0, 0.0, 10, 0.6, Color(1.0, 0.0, 0.0), 0.9, 1.4);
    Material* ball5_mat = new Material(Color::white(), 0.0, 0.0, 10, 0.6, Color(0.0, 1.0, 0.0), 0.9, 1.4);
    Material* ball6_mat = new Material(Color::white(), 0.0, 0.0, 10, 0.6, Color(0.0, 0.0, 1.0), 0.9, 1.4);
    Material* floor_mat = new Material(Color::white(), 1.0, 0.2, 6, 0.6, Color(1.0, 1.0, 1.0), 0.0, 1.0);
    floor_mat->color = Color::white() * 0.8;
    floor_mat->tex = readTex("tex/board.bmp");

    Plane* floor = new Plane(
        Vector3(0.0, 0.0, 0.0),
        Vector3(1.0, 0.0, 0.0),
        Vector3(0.0, 0.0, -1.0),
        floor_mat);
    scene2->prim.push_back(floor);

    scene2->prim.push_back(new Sphere(Vector3(-4.0, 1.0, 0.0), 1.0, 0.0, 0.0, ball1_mat));
    scene2->prim.push_back(new Sphere(Vector3(0.0, 1.0, 0.0), 1.0, 0.0, 0.0, ball2_mat));
    scene2->prim.push_back(new Sphere(Vector3(4.0, 1.0, 0.0), 1.0, 0.0, 0.0, ball3_mat));
    scene2->prim.push_back(new Sphere(Vector3(-4.0, 1.0, 4.0), 1.0, 0.0, 0.0, ball6_mat));
    scene2->prim.push_back(new Sphere(Vector3(0.0, 1.0, 4.0), 1.0, 0.0, 0.0, ball5_mat));
    scene2->prim.push_back(new Sphere(Vector3(4.0, 1.0, 4.0), 1.0, 0.0, 0.0, ball4_mat));

    scene2->light.push_back(new AreaLight(80, Color::white(), 
        (Transform::Rotate(0,0,1,PI/2) * Transform::Scale(2.0)).translate_(-10.0, 5.0, -2.0)));

    scene2->light.push_back(new AreaLight(80, Color::white(), 
        (Transform::Rotate(0,0,1,-PI/2) * Transform::Scale(2.0)).translate_(10.0, 5.0, -2.0)));

    scene2->light.push_back(new AreaLight(50, Color::white(), 
        Transform::Scale(2.0).translate_(0.0, 5.0, 4.0)));
    scene2->light.push_back(new AreaLight(30, Color::white(), 
        Transform::Rotate(1,0,0,-PI/6) * Transform::Scale(2.0).translate_(0.0, 5.0, 0.0)));

    return scene2;
}

Scene* Scene3() {
    Scene* scene3 = new Scene();

    scene3->camera.set(
        Vector3(0.0, 4.0, -3.0),
        Vector3(0.0, 1.0, 0.0),
        Vector3(0.0, -0.72, 1.0)
    );

    Material* ball1_mat = new Material(Color::white(), 0.0, 0.0, 10, 0.3, Color(0.0, 1.0, 1.0), 0.95, 1.4);
    Material* ball2_mat = new Material(Color::white(), 0.0, 0.0, 10, 0.3, Color(1.0, 0.0, 1.0), 0.95, 1.4);
    Material* ball3_mat = new Material(Color::white(), 0.0, 0.0, 10, 0.3, Color(1.0, 1.0, 0.0), 0.95, 1.4);
    Material* ball4_mat = new Material(Color::white(), 0.0, 0.0, 10, 0.3, Color(1.0, 0.0, 0.0), 0.95, 1.4);
    Material* ball5_mat = new Material(Color::white(), 0.0, 0.0, 10, 0.3, Color(0.0, 1.0, 0.0), 0.95, 1.4);
    Material* ball6_mat = new Material(Color::white(), 0.0, 0.0, 10, 0.3, Color(0.0, 0.0, 1.0), 0.95, 1.4);
    Material* floor_mat = new Material(Color::white(), 1.0, 0.1, 6, 0.6, Color(1.0, 1.0, 1.0), 0.0, 1.0);
    floor_mat->color = Color::white() * 0.8;
    //floor_mat->tex = readTex("tex/board.bmp");
    floor_mat->tex = readTex("tex/wood.bmp");

    Plane* floor = new Plane(
        Vector3(0.0, 0.0, 0.0),
        Vector3(16.0, 0.0, 0.0),
        Vector3(0.0, 0.0, -16.0),
        floor_mat);
    scene3->prim.push_back(floor);

    scene3->prim.push_back(new Sphere(Vector3(-4.0, 1.0, 0.0), 1.0, 0.0, 0.0, ball1_mat));
    scene3->prim.push_back(new Sphere(Vector3(0.0, 1.0, 0.0), 1.0, 0.0, 0.0, ball2_mat));
    scene3->prim.push_back(new Sphere(Vector3(4.0, 1.0, 0.0), 1.0, 0.0, 0.0, ball3_mat));
    scene3->prim.push_back(new Sphere(Vector3(-4.0, 1.0, 4.0), 1.0, 0.0, 0.0, ball6_mat));
    scene3->prim.push_back(new Sphere(Vector3(0.0, 1.0, 4.0), 1.0, 0.0, 0.0, ball5_mat));
    scene3->prim.push_back(new Sphere(Vector3(4.0, 1.0, 4.0), 1.0, 0.0, 0.0, ball4_mat));

    //scene3->light.push_back(new PointLight(20, Color::white(),
    //    Vector3(2.0, 3.0, 2.0)));
    //scene3->light.push_back(new PointLight(20, Color::white(),
    //    Vector3(-2.0, 3.0, 2.0)));
    //scene3->light.push_back(new PointLight(20, Color::white(),
    //    Vector3(0.0, 3.0, 2.0)));
    scene3->light.push_back(new PointLight(80, Color::white(),
        Vector3(1.0, 3.0, 3.0), 0.1));

    //scene3->light.push_back(new AreaLight(80, Color::white(),
    //    (Transform::Rotate(0, 0, 1, PI / 2) * Transform::Scale(2.0)).translate_(-10.0, 5.0, -2.0)));

    //scene3->light.push_back(new AreaLight(80, Color::white(),
    //    (Transform::Rotate(0, 0, 1, -PI / 2) * Transform::Scale(2.0)).translate_(10.0, 5.0, -2.0)));

    //scene3->light.push_back(new AreaLight(50, Color::white(),
    //    Transform::Scale(1.0).translate_(0.0, 3.0, 3.0)));
    //scene3->light.push_back(new AreaLight(30, Color::white(),
    //    Transform::Rotate(1, 0, 0, -PI / 6) * Transform::Scale(2.0).translate_(0.0, 5.0, 0.0)));

    return scene3;
}

Scene* Scene4() {
    Scene* scene4 = new Scene();

    scene4->camera.set(Vector3(0.0, 9, 0.0), Vector3(0.0, 0.0, 1.0), Vector3(0.0, -1.0, 0.0));

    double n = 1.414;
    Material* ball_mat[12];
    ball_mat[0] = new Material(Color::white(), 0, 0, 10, 0.3, Color(0.0, 1.0, 1.0), 0.95, n);
    ball_mat[1] = new Material(Color::white(), 0, 0, 10, 0.3, Color(0.0, 0.5, 1.0), 0.95, n);
    ball_mat[2] = new Material(Color::white(), 0, 0, 10, 0.3, Color(0.0, 0.0, 1.0), 0.95, n);
    ball_mat[3] = new Material(Color::white(), 0, 0, 10, 0.3, Color(0.5, 0.0, 1.0), 0.95, n);
    ball_mat[4] = new Material(Color::white(), 0, 0, 10, 0.3, Color(1.0, 0.0, 1.0), 0.95, n);
    ball_mat[5] = new Material(Color::white(), 0, 0, 10, 0.3, Color(1.0, 0.0, 0.5), 0.95, n);
    ball_mat[6] = new Material(Color::white(), 0, 0, 10, 0.3, Color(1.0, 0.0, 0.0), 0.95, n);
    ball_mat[7] = new Material(Color::white(), 0, 0, 10, 0.3, Color(1.0, 0.5, 0.0), 0.95, n);
    ball_mat[8] = new Material(Color::white(), 0, 0, 10, 0.3, Color(1.0, 1.0, 0.0), 0.95, n);
    ball_mat[9] = new Material(Color::white(), 0, 0, 10, 0.3, Color(0.5, 1.0, 0.0), 0.95, n);
    ball_mat[10]= new Material(Color::white(), 0, 0, 10, 0.3, Color(0.0, 1.0, 0.0), 0.95, n);
    ball_mat[11]= new Material(Color::white(), 0, 0, 10, 0.3, Color(0.0, 1.0, 0.5), 0.95, n);
    //ball_mat[12]= new Material(Color::white(), 0, 0, 10, 0.0, Color(0.0, 0.0, 0.0), 0.95, 1.0);

    Material* floor_mat = new Material(Color::white(), 1.0, 0.1, 6, 0.6, Color(1.0, 1.0, 1.0), 0.0, 1.0);
    floor_mat->color = Color::white() * 0.8;
    floor_mat->tex = readTex("tex/board.bmp");
    //floor_mat->tex = readTex("tex/wood.bmp");

    Plane* floor = new Plane(
        Vector3(0.0, 0.0, 0.0),
        Vector3(1.0, 0.0, 0.0),
        Vector3(0.0, 0.0, -1.0),
        floor_mat);
    scene4->prim.push_back(floor);

    for(int i = 0; i < 12; ++i) {
        double theta = (2 * i + 1) * PI / 12;
        scene4->prim.push_back(new Sphere(Vector3(3.0 * sin(theta), 1.0, 3.0 * cos(theta)), 0.6, 0.0, 0.0, ball_mat[i]));
    }

    //scene4->prim.push_back(new Sphere(Vector3(0.0, 1.0, 0.0), 0.6, 0.0, 0.0, ball_mat[12]));


    scene4->light.push_back(new PointLight(100, Color::white(),
        Vector3(0.0, 3.5, 0.0), 0.1));

    return scene4;
}

Scene* Scene5() {
    Scene* scene5 = new Scene();

    scene5->camera.set(
        Vector3(0.0, 2.0, -4.0),
        Vector3(0.0, 1.0, 0.0),
        Vector3(0.0, 0, 0.72)
    );

    Material* goblet_mat = new Material(Color::white(), 0.3, 0.2, 10, 0.5, Color(0.1, 0.1, 0.1), 0.9, 1.0);
    Material* floor_mat = new Material(Color::white(), 1.0, 0.1, 5, 0.4, Color(1.0, 1.0, 1.0), 0.0, 1.0);
    floor_mat->color = Color::white() * 0.8;
    floor_mat->tex = readTex("tex/board.bmp");
    //floor_mat->tex = readTex("tex/wood.bmp");

    Plane* floor = new Plane(
        Vector3(0.0, 0.0, 0.0),
        Vector3(1.0, 0.0, -1.0),
        Vector3(-1.0, 0.0, -1.0),
        floor_mat);
    scene5->prim.push_back(floor);

    Bezier_rotated* goblet = new Bezier_rotated(goblet_mat);
    goblet->setTransform(Transform::Rotate(1.0, 0.0, 0.0, -PI/2));
    //goblet->addPoint(1.0, 1.00);
    //goblet->addPoint(1.0, 0.00);
    goblet->addPoint(0.718, 3.00);
    goblet->addPoint(1.000, 1.94);
    goblet->addPoint(0.824, 1.33);
    goblet->addPoint(0.580, 1.19);
    goblet->addPoint(0.344, 1.22);
    goblet->addPoint(0.600, 1.12);
    goblet->addPoint(0.700, 0.00);
    scene5->prim.push_back(goblet);

    //goblet->addPoint(0.500, 1.00);
    //goblet->addPoint(1.000, 0.75);
    //goblet->addPoint(1.000, 0.50);
    //goblet->addPoint(0.250, 0.25);
    //goblet->addPoint(0.800, 0.00);

    scene5->light.push_back(new PointLight(60, Color::white(), Vector3(3.0, 5.0, -2.0)));
    
    return scene5;
}