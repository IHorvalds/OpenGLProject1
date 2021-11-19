#include <GL/glew.h> 
#include <GL/freeglut.h>

#include "Shader.h"

#include <vector>

#include "glm/glm.hpp"  
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Object.h"

#include <iostream>

int width = 1300, height = 800;

std::vector<Vertex> carA({
    //                Position               ||         Colour
    // Car A.
    Vertex{ glm::vec4(180.f, 50.f, 0.f, 1.f), glm::vec4(0.f, 1.f, 0.f, 1.f), glm::vec2(0.f, 0.f) },
    Vertex{ glm::vec4(180.f, 90.f, 0.f, 1.f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(0.f, 1.f) },
    Vertex{ glm::vec4(260.f, 50.f, 0.f, 1.f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(1.f, 0.f) },
    Vertex{ glm::vec4(260.f, 90.f, 0.f, 1.f), glm::vec4(0.f, 1.f, 0.f, 1.f), glm::vec2(1.f, 1.f) },
});

std::vector<Vertex> carB({
    // Car B.
    Vertex{ glm::vec4(180.f, 100.f, 0.f, 1.f), glm::vec4(0.f, .2f, .6f, 1.f), glm::vec2(0.f, 0.f)},
    Vertex{ glm::vec4(180.f, 140.f, 0.f, 1.f), glm::vec4(0.f, .2f, .6f, 1.f), glm::vec2(0.f, 1.f)},
    Vertex{ glm::vec4(260.f, 100.f, 0.f, 1.f), glm::vec4(0.f, .2f, .6f, 1.f), glm::vec2(1.f, 0.f)},
    Vertex{ glm::vec4(260.f, 140.f, 0.f, 1.f), glm::vec4(0.f, .2f, .6f, 1.f), glm::vec2(1.f, 1.f)}
});

std::vector<Vertex> raceTrack({
    // Race Track.
    Vertex{ glm::vec4(0.f,          0.f,           0.f, 1.f), glm::vec4(0.f, 0.f, 0.f, 1.f), glm::vec2(0.f, 0.f) },
    Vertex{ glm::vec4(0.f,          (float)height, 0.f, 1.f), glm::vec4(0.f, 0.f, 0.f, 1.f), glm::vec2(0.f, 1.f) },
    Vertex{ glm::vec4((float)width, 0.f,           0.f, 1.f), glm::vec4(0.f, 0.f, 0.f, 1.f), glm::vec2(1.f, 0.f) },
    Vertex{ glm::vec4((float)width, (float)height, 0.f, 1.f), glm::vec4(0.f, 0.f, 0.f, 1.f), glm::vec2(1.f, 1.f) }
    });

std::vector<GLuint> carA_indices({
    0, 1, 3, 3, 2, 0,
});

std::vector<GLuint> carB_indices({
    0, 1, 3, 3, 2, 0,
});

std::vector<GLuint> race_track_indices({
    0, 1, 3, 3, 2, 0,
});

const glm::mat4 identity = glm::mat4(1.f);
glm::mat4 resMatrix, trslMatrix;
const float ang2rad = 3.14159265359f / 180.f;

Shader* shader;

Object* carAObject, *carBObject, *raceTrackObject;
float speed_multiplier = 1.f;


enum OvertakeStage {
    LEFT,
    FORWARD, 
    RIGHT,
    BACKWARD,
    STEADY,
    BREAK
};

OvertakeStage stage = STEADY;
int is_forward = 1;

enum SpeedStep {
    SPEED1,
    SPEED2,
    SPEED3,
    SPEED4
};
SpeedStep current_speed_step = SPEED1;

enum CarBStages {
    STRAIGHT,
    LEFT_90DEG_CORNER,
    RIGHT_SHORT,
    LEFT_SHORT,
    LEFT_180_DEG_CORNER,
    RIGHT_180_DEG_CORNER
};

CarBStages lap[] = {
    STRAIGHT, LEFT_90DEG_CORNER, STRAIGHT, LEFT_90DEG_CORNER,
    STRAIGHT, RIGHT_SHORT, STRAIGHT, LEFT_SHORT, STRAIGHT, LEFT_SHORT, STRAIGHT, RIGHT_SHORT,
    STRAIGHT, LEFT_180_DEG_CORNER, STRAIGHT, RIGHT_180_DEG_CORNER, STRAIGHT, LEFT_180_DEG_CORNER
};

int current_car_b_stage = -1;
int turn_count = 0;
float old_t;

void init()
{

    // Resize matrix intialization
    resMatrix = glm::scale(identity, glm::vec3(2.f / width, 2.f / height, 1.f));
    trslMatrix = glm::translate(identity, glm::vec3(-width/2, -height/2, 0.f));

    // Car A
    carAObject = new Object(carA, carA_indices, "red_race_car.png");
    
    // Car B
    carBObject = new Object(carB, carB_indices, "blue_race_car.png");

    // Race Track
    raceTrackObject = new Object(raceTrack, race_track_indices, "racetrack.png");


    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    shader->Activate();
}

void Reset()
{
    carAObject->Reset();
    carBObject->Reset();
    current_speed_step = SPEED1;
    current_car_b_stage = -1;
    turn_count = 0;
}

void HandleMouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        Reset();
    }
}
void HandleNormalKeys(unsigned char key, int x, int y)
{
    if (key == ' ') {
        Reset();
    }

    if (key == 'b') {
        stage = BREAK;
    }

    if (key == 's') {
        Reset();
        current_car_b_stage = 0;
    }
}
void HandleKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT)
    {
        stage = LEFT;
    }
    if (key == GLUT_KEY_RIGHT)
    {
        stage = RIGHT;
    }
    if (key == GLUT_KEY_UP)
    {
        stage = FORWARD;
        is_forward = 1;
    }
    if (key == GLUT_KEY_DOWN)
    {
        stage = BACKWARD;
        is_forward = -1;
    }
}

void SetSpeedMultiplier()
{
    if (stage == FORWARD) {
        if (current_speed_step < SPEED4)
            current_speed_step = (SpeedStep)((current_speed_step + 1) % 4);
    }
    if (stage == BREAK) {
        if (current_speed_step > SPEED1)
            current_speed_step = (SpeedStep)((current_speed_step - 1) % 4);
    }
    if (stage == BACKWARD) {
        if (current_speed_step < SPEED2)
            current_speed_step = (SpeedStep)((current_speed_step + 1) % 4);
    }
    switch (current_speed_step)
    {
        case SPEED1:
            if (stage != BREAK)
                speed_multiplier = 1.f;
        break;
        case SPEED2:
            speed_multiplier = 1.3f;
        break;
        case SPEED3:
            speed_multiplier = 2.f;
        break;
        case SPEED4:
            speed_multiplier = 3.f;
        break;
    }
}

void carATransform(glm::mat4& transform, glm::vec3& current_position, glm::vec3& force, glm::vec3& rotationCenter, float& local_speed, float& local_rotation)
{
    switch (stage)
    {
    case LEFT:
        local_rotation += 10.f;
        force.x = std::cos(ang2rad * local_rotation) * is_forward * local_speed * speed_multiplier;
        break;
    case RIGHT:
        local_rotation -= 10.f;
        force.x = std::cos(ang2rad * local_rotation) * is_forward * local_speed * speed_multiplier;
        break;
    case FORWARD:
        SetSpeedMultiplier();
        force.x = std::cos(ang2rad * local_rotation) * is_forward * local_speed * speed_multiplier;
        break;
    case BACKWARD:
        SetSpeedMultiplier();
        force.x = std::cos(ang2rad * local_rotation) * is_forward * local_speed * speed_multiplier;
        break;
    case BREAK:
        SetSpeedMultiplier();
        if (current_speed_step == SPEED1)
            if (speed_multiplier <= 0.9f) {
                speed_multiplier *= 0.9f;
            }
            else {
                speed_multiplier = 0.9;
            }
        force.x = std::cos(ang2rad * local_rotation) * is_forward * local_speed * speed_multiplier;
        break;
    case STEADY:
        break;
    }
    stage = STEADY;

    force.y = std::sin(ang2rad * local_rotation) * local_speed * speed_multiplier * is_forward;

    current_position += force;

    glm::mat4 toCenter = glm::translate(identity, current_position) * glm::translate(identity, rotationCenter) * glm::rotate(identity, ang2rad * local_rotation, glm::vec3(0.f, 0.f, 1.f)) * glm::translate(identity, -rotationCenter);

    // return via parameter
    transform = toCenter;
}

void carBTransform(glm::mat4& transform, glm::vec3& current_position, glm::vec3& force, glm::vec3& rotationCenter, float& local_speed, float& local_rotation)
{
    float t = glutGet(GLUT_ELAPSED_TIME);
    const float local_speed_multiplier = 1.5f;
    switch (current_car_b_stage)
    {
    case 0:
        force.x = std::cos(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
        if (carBObject->GetTransform()[3][0] > 820)
            current_car_b_stage++;
        break;
    case 1:
        if (turn_count < 9) {
            if ((t - old_t) / 1000.f >= 1.f / 7.f) {
                old_t = t;
                local_rotation += 10.f;
                force.x = std::cos(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
                turn_count++;
            }
        }
        else {
            turn_count = 0;
            current_car_b_stage++;
        }
        break;
    case 2:
        force.x = std::cos(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
        if (carBObject->GetTransform()[3][1] > 330)
            current_car_b_stage++;
        break;
    case 3:
        if (turn_count < 9) {
            if ((t - old_t) / 1000.f >= 1.f / 7.f) {
                old_t = t;
                local_rotation += 10.f;
                force.x = std::cos(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
                turn_count++;
            }
        }
        else {
            turn_count = 0;
            current_car_b_stage++;
        }
        break;
    case 4:
        force.x = std::cos(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
        if (carBObject->GetTransform()[3][0] < 1000)
            current_car_b_stage++;
        break;
    case 5:
        if (turn_count < 3) {
            if ((t - old_t) / 1000.f >= 1.f / 7.f) {
                old_t = t;
                local_rotation -= 10.f;
                force.x = std::cos(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
                turn_count++;
            }
        }
        else {
            turn_count = 0;
            current_car_b_stage++;
        }
        break;
    case 6:
        force.x = std::cos(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
        if (carBObject->GetTransform()[3][0] < 900)
            current_car_b_stage++;
        break;
    case 7:
        if (turn_count < 3) {
            if ((t - old_t) / 1000.f >= 1.f / 7.f) {
                old_t = t;
                local_rotation += 10.f;
                force.x = std::cos(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
                turn_count++;
            }
        }
        else {
            turn_count = 0;
            current_car_b_stage++;
        }
        break;
    case 8:
        force.x = std::cos(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
        if (carBObject->GetTransform()[3][0] < 750)
            current_car_b_stage++;
        break;
    case 9:
        if (turn_count < 3) {
            if ((t - old_t) / 1000.f >= 1.f / 7.2f) {
                old_t = t;
                local_rotation += 10.f;
                force.x = std::cos(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
                turn_count++;
            }
        }
        else {
            turn_count = 0;
            current_car_b_stage++;
        }
        break;
    case 10:
        force.x = std::cos(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
        if (carBObject->GetTransform()[3][0] < 550)
            current_car_b_stage++;
        break;
    case 11:
        if (turn_count < 3) {
            if ((t - old_t) / 1000.f >= 1.f / 7.f) {
                old_t = t;
                local_rotation -= 10.f;
                force.x = std::cos(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
                turn_count++;
            }
        }
        else {
            turn_count = 0;
            current_car_b_stage++;
        }
        break;
    case 12:
        force.x = std::cos(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
        if (carBObject->GetTransform()[3][0] < 450)
            current_car_b_stage++;
        break;
    case 13:
        if (turn_count < 18) {
            if ((t - old_t) / 1000.f >= 1.f / 11.f) {
                old_t = t;
                local_rotation += 10.f;
                force.x = std::cos(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
                turn_count++;
            }
        }
        else {
            turn_count = 0;
            current_car_b_stage++;
        }
        break;
    case 14:
        force.x = std::cos(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
        if (carBObject->GetTransform()[3][0] > 450)
            current_car_b_stage++;
        break;
    case 15:
        if (turn_count < 18) {
            if ((t - old_t) / 1000.f >= 1.f / 11.f) {
                old_t = t;
                local_rotation -= 10.f;
                force.x = std::cos(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
                turn_count++;
            }
        }
        else {
            turn_count = 0;
            current_car_b_stage++;
        }
        break;
    case 16:
        force.x = std::cos(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
        if (carBObject->GetTransform()[3][0] < 430)
            current_car_b_stage++;
        break;
    case 17:
        if (turn_count < 18) {
            if ((t - old_t) / 1000.f >= 1.f / 10.f) {
                old_t = t;
                local_rotation += 10.f;
                force.x = std::cos(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
                turn_count++;
            }
        }
        else {
            turn_count = 0;
            current_car_b_stage = 0;
        }
        break;
    case -1:
        force = glm::vec3(0.f);
        break;
    }

    force.y = std::sin(ang2rad * local_rotation) * local_speed * local_speed_multiplier;
    current_position += force;

    glm::mat4 toCenter = glm::translate(identity, current_position) * glm::translate(identity, rotationCenter) * glm::rotate(identity, ang2rad * local_rotation, glm::vec3(0.f, 0.f, 1.f)) * glm::translate(identity, -rotationCenter);
    transform = toCenter;
}

void raceTrackTransform(glm::mat4& transform, glm::vec3& current_position, glm::vec3& force, glm::vec3& rotationCenter, float& local_speed, float& local_rotation)
{
    transform = identity;
}

void CollisionDetection()
{
    // just between the 2 cars. Not really extensible
    
    std::vector<glm::vec4> w_calculated(4);
    for (auto w : carB_indices)
    {   
        w_calculated[w] = resMatrix * trslMatrix * carBObject->GetTransform() * carB[w].position;
    }
    for (auto v : carA_indices)
    {
        glm::vec4 adjusted_v = resMatrix * trslMatrix * carAObject->GetTransform() * carA[v].position;
    
        

        if (adjusted_v.x >= w_calculated[0].x && adjusted_v.x <= w_calculated[2].x) {
            if(adjusted_v.y >= w_calculated[0].y && adjusted_v.y <= w_calculated[1].y) {
                std::cout << "Hit!" << "\n";
                Reset();
            }
        }
    }
}

void RenderFunction()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    glm::mat4 resizeMatrix = resMatrix * trslMatrix;
    raceTrackObject->Draw(resizeMatrix, shader, raceTrackTransform);
    carAObject->Draw(resizeMatrix, shader, carATransform);
    carBObject->Draw(resizeMatrix, shader, carBTransform);
    CollisionDetection();
    glutSwapBuffers();

    glutPostRedisplay();
}


void Cleanup()
{
    shader->Delete();
    delete(carAObject);
    delete(carBObject);
    delete(raceTrackObject);
    delete(shader);
}

int main(int argc, char** argv)
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width , height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Proiect Croitoru");
    glewInit();
    shader = new Shader("default.vert", "default.frag");

    init();
    old_t = glutGet(GLUT_ELAPSED_TIME);
    glutDisplayFunc(RenderFunction);
    glutMouseFunc(HandleMouse);
    glutKeyboardFunc(HandleNormalKeys);
    glutSpecialFunc(HandleKeys);
    glutCloseFunc(Cleanup);


    glutMainLoop();

    return 0;
}