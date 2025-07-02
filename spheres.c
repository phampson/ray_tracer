// C program to demonstrate
// drawing a circle using
// OpenGL
// compile with : gcc spheres.c -lGL -lGLU -lglut -lm
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<GL/freeglut.h>
#include<math.h>
#include "graphics_objects.h"
#define pi 3.142857


float viewport_size = 1;// 1 x 1;

float projection_plane_d = 1;

float Vh = 2;
float Vw = 3;

float Ch = 1000;
float Cw = 1500;

float O[] = {0, 0, 0};

float Yaw = 0; // For now lets keep in multiples of 10

float distance = 1;


Sphere spheres[] = {s1, s2, s3, s4};
Light lights[] = {l1, l2, l3};


// function to initialize
void myInit (void)
{
    // making background color black as first 
    // 3 arguments all are 0.0
    glClearColor(0.0, 0.0, 0.0, 1.0);

    // breadth of picture boundary is 1 pixel
    glPointSize(1.0);
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity();
    
    // setting window dimension in X- and Y- direction
    gluOrtho2D(-Cw/2, Cw/2, -Ch/2, Ch/2);
}


float dotProduct(const float* a, const float* b){
    float result = 0;

    for (int i = 0; i < 3; i++) {
        result += (a[i] * b[i]);
    }

    return result;
}


float vectorLength(float* v) {
    return sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}


void intersectRaySphere(float* O, float* D, Sphere sphere, float* intersection_array){
    float radius = sphere.radius; // Point
    float CO[3] = {
        O[0] - sphere.center[0],
        O[1] - sphere.center[1],
        O[2] - sphere.center[2]
    }; //O - sphere.center; // Vector: Origin - Sphere center 

    // Three variables in quadratic equation.
    float a = dotProduct(D, D); 
    float b = 2 * dotProduct(CO, D);
    float c = dotProduct(CO, CO) - sphere.radius*sphere.radius;

    float discriminant = b*b - 4*a*c;

    if (discriminant < 0) {
        intersection_array[0] = INFINITY;
        intersection_array[1] = INFINITY;
    } else {
        intersection_array[0] = (-b + sqrt(discriminant)) / (2*a); // t1
        intersection_array[1] = (-b - sqrt(discriminant)) / (2*a); // t2
    }
}


Intersection closestIntersection(float* O, float* D, float t_min, float t_max) {
    Intersection result;
    result.closest_t = INFINITY;
    result.closest_sphere = NULL;

    for (int i = 0; i < 4; i += 1) {
        float intersection_array[2];   

        intersectRaySphere(O, D, spheres[i], intersection_array);

        float t1 = intersection_array[0];
        float t2 = intersection_array[1];

        if (t_min < t1 && t1 < t_max && t1 < result.closest_t) {
            result.closest_t = t1;
            result.closest_sphere = &spheres[i];
        }

        if (t_min < t2 && t2 < t_max && t2 < result.closest_t) {
            result.closest_t = t2;
            result.closest_sphere = &spheres[i];
        }
    }

    return result;
}


float computeLighting(float* P, float* N, float* V, float s) {
    float lighting_multiplier = 0.0; 
    float t_max;

    for (int i = 0; i < 3; i += 1) {
        Light light = lights[i];

        if (strcmp(light.type, "ambient") == 0) {
            lighting_multiplier += light.intensity;
        } else {
            float L[3];

            if (strcmp(light.type, "point") == 0) {
                // L = light.position - P;
                for (int j = 0; j < 3; j += 1) {
                    L[j] = light.position[j] - P[j]; 
                }
                // Set t_max
                t_max = 1;
            } else {
                // L = light.direction;
                for (int j = 0; j < 3; j += 1) {
                    L[j] = light.direction[j];
                }
                // Set t_max
                t_max = INFINITY;
            }

            // Shadow check
            Intersection intersection;
            intersection = closestIntersection(P, L, 0.001, t_max);

            if (intersection.closest_sphere != NULL) {
                continue;
            }

            // Diffuse
            float n_dot_l = dotProduct(N, L);
            if (n_dot_l > 0) {
                lighting_multiplier += light.intensity * n_dot_l/(vectorLength(N) * vectorLength(L));
            }


            // Specular
            if (s != -1) {
                float R[3];
                for (int j = 0; j < 3; j += 1){
                    R[j] = 2 * N[j] * dotProduct(N, L) - L[i];
                }

                float r_dot_v = dotProduct(R, V);
                
                if (r_dot_v > 0) {
                    lighting_multiplier += light.intensity * pow(r_dot_v/(vectorLength(R) * vectorLength(V)), s);
                }
            }
        }
    }

    return lighting_multiplier;
}


void reflectRay(float* target, float* R, float* N) {
    float dot_n_r = dotProduct(R, N);
    for (int i = 0; i < 3; i += 1) {
        target[i] = 2 * N[i] * dot_n_r - R[i];
    }
}


Color traceRay(float* O, float* D, float t_min, float t_max, float recursion_depth) {
    // returns array that represents the RGB values
    Color color;

    Intersection intersection;
    intersection = closestIntersection(O, D, t_min, t_max);

    if (intersection.closest_sphere == NULL){
        color.r = 0;
        color.g = 0;
        color.b = 0;

        return color;
    } else {
        // Get color now that we've hit an object
        float P[3];  // Compute intersection
        for (int i = 0; i < 3; i += 1) {
            P[i] = O[i] + (intersection.closest_t * D[i]);
        }

        float N[3]; // Compute sphere normal at intersection
        for (int i = 0; i < 3; i += 1) {
            N[i] = P[i] - intersection.closest_sphere->center[i];
        }  
        
        float length_n = vectorLength(N);

        for (int i = 0; i < 3; i += 1) {
            N[i] = N[i] / length_n;
        }
        
        float V[3];
        for (int i = 0; i < 3; i += 1) {
            V[i] = -1 * D[i];
        }

        float lighting_multiplier = computeLighting(P, N, V, intersection.closest_sphere->specular);

        color.r = intersection.closest_sphere->color[0] * lighting_multiplier;
        color.g = intersection.closest_sphere->color[1] * lighting_multiplier;
        color.b = intersection.closest_sphere->color[2] * lighting_multiplier;

        // Recursion trigger before we set the color
        float r = intersection.closest_sphere->reflective;
        if (recursion_depth <= 0 || r <= 0) {
            return color;
        }

        // TODO: I should find a pre canned vector or make one for my needs
        float reverse_D[] = { // This is -D
            -1 * D[0],
            -1 * D[1],
            -1 * D[2]
        };

        float R[3];
        reflectRay(R, reverse_D, N);
        Color reflected_color = traceRay(P, R, 0.001, INFINITY, recursion_depth - 1);

        color.r = color.r * (1 - r) + reflected_color.r * r;
        color.g = color.g * (1 - r) + reflected_color.g * r;
        color.b = color.b * (1 - r) + reflected_color.b * r;

        return color;
    }
}


void canvasToViewport(const float x, const float y, float* D) {
    // x times Viewport width / Canvas width. Same with height
    float temp_D[3];

    temp_D[0] = x*(Vw/Cw);
    temp_D[1] = y*(Vh/Ch);
    temp_D[2] = distance;

    float R[3][3] = {
        {cos(Yaw * pi), 0, sin(Yaw * pi)},
        {0, 1, 0},
        {-1 * sin(Yaw * pi), 0, cos(Yaw * pi)}
    };

    for (int i = 0; i < 3; i += 1) {
        D[i] = 0;
        for (int j = 0; j < 3; j += 1) {
            D[i] += R[i][j] * temp_D[j];
        }
    }
}


void putPixel(Color color, int x, int y) {
    glColor3f(color.r, color.g, color.b);
    glVertex2i(x, y);
}


void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);  // Set position in window (orthographic coords)
    for (int i = 0; i < strlen(text); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}


void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POINTS);

    float D[3];
    // float O[] = {0,0,0};
    Color color;

    for (int x=-Cw/2; x<Cw/2; x+=1){
        for (int y=-Ch/2; y<Ch/2; y+=1){
            canvasToViewport(x, y, D); // assigns D 
            color = traceRay(O, D, 1, INFINITY, 5);
            putPixel(color, x, y);
        }
    }

    glEnd();
    glFlush();

    //drawText(0,0, "Hello, OpenGL!");
}


void handleKeypress(unsigned char key, int x, int y) {
    if (key == 'w' || key == 'W') { 
        printf("W\n");
        O[1] += 1;
    }
    if (key == 's' || key == 'S') { 
        printf("S\n");
        O[1] -= 1;
    }
    if (key == 'a' || key == 'A') {
        Yaw = Yaw - 0.1;
        if (Yaw < 0) {
            Yaw = 1.9;
        }
        printf("Yaw is %f\n", Yaw);
    }
    if (key == 'd' || key == 'D') {
        Yaw = fmod((Yaw + 0.1), 2);
        printf("Yaw is %f\n", Yaw);
    }


    glutPostRedisplay();
}


// Called when a special key is pressed (e.g., arrow keys)
void handleSpecialKey(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            printf("Arrow Up\n");
            O[2] += 1;
            break;
        case GLUT_KEY_DOWN:
            printf("Arrow Down\n");
            O[2] -= 1;
            break;
        case GLUT_KEY_LEFT:
            printf("Arrow Left\n");
            O[0] -= 1;
            break;
        case GLUT_KEY_RIGHT:
            printf("Arrow Right\n");
            O[0] += 1;
            break;
    }

    glutPostRedisplay();
}


void handleMenu(int choice) {
    switch (choice) {
        case 1:
            printf("Reset to Origin\n");
            O[0] = 0;
            O[1] = 0;
            O[2] = 0;
            Yaw = 0;
            glutPostRedisplay();
            break;
        case 2:
            printf("Reset Yaw\n");
            Yaw = 0;
            glutPostRedisplay();
            break;
        case 3:
            exit(0);
    }
}


int main (int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    
    int menu = glutCreateMenu(handleMenu);
    glutAddMenuEntry("Reset to Origin", 1);
    glutAddMenuEntry("Reset Yaw", 2);
    glutAddMenuEntry("Exit", 3);


    // giving window size in X- and Y- direction
    glutInitWindowSize(Cw, Ch);
    glutInitWindowPosition(0, 0);
    // didnt work glPointSize(1.0);

    // Giving name to window
    glutCreateWindow("Spheres");
    myInit();

    glutDisplayFunc(display); // Called when the window must be (re)drawn?
    glutKeyboardFunc(handleKeypress);
    glutSpecialFunc(handleSpecialKey);
    glutAttachMenu(GLUT_LEFT_BUTTON);
    glutMainLoop(); // This is a non stop loop that keeps claling display
}
