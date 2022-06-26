# Projects 3: Ray

### Introduction to Computer Graphics, Fall 2022

*Due:*


## 1 Introduction

In the previous two labs, you have already learned about transformation on shape primitives, how cameras work, and how to parse and load a scene. Now it’s the time to link all these together and build something greater! 

In this assignment, you will implement the basic part of your ray tracer, an intersection check pipeline that simulates how ray bounces in a scene. 

**The overview of ray tracing pipeline**

In lectures, you’ve learned that the ray tracing pipeline can be roughly divided into these three components. However, you are only required to focus on the first two components in the scope of this assignment.

- Generating and casting rays
- Finding the closest intersection point
- Compute the illumination at the intersection point

## 2 Requirements

### 2.1 Generating and casting rays

In order to generate and cast rays into the scene, you will need to shoot rays through a pixel. For basic requirements, **you are only required to shoot one ray through the center of each pixel**. 

Of course, this will cause aliasing, which you have encountered before in lectures and in the last assignment. There are various ways to smoothen the output. Check the extra credit section if you are interested.

### 2.2 Finding intersection point

In order to find an intersection point, we first have to define our shapes. As covered in lectures, we are going to use implicit functions to define our shapes. You are required to be able to correctly handle the following types of shapes using implicit functions. 

- Cube
- Cone
- Cylinder
- Sphere

### 2.3 “Illumination”
In this assignment, you are not required to handle any lighting. Yet, it would still be nice to be able to distinguish between different primitives in the scene and verify the implementation. To do this, **simply use the normal value as the color**.

You may want to check the data structures in `CS123SceneData.h` and make sure you fully understand all of them.

### 2.4 Rubrics

This assignment is out of 100 points

* Four implicit shape primitives **(40pts)**:
  * Cube **(10pts)**
  * Sphere **(10pts)**
  * Cylinder **(10pts)**
  * Cone **(10pts)**
* Intersection check pipeline **(30pts)**
* Efficiency **(15pts)**
* Software Engineering & Stability **(10pts)**
* README **(5pts)**

## 3 Codebase & Design

You may have noticed that only a limited amount of support code is provided for this assignment. Hence, it means that it’s up to you to make any design choice for your project. You have all the freedom to add any additional file that you think is necessary for this assignment.

Please do keep in mind that you are only implementing a part of your ray tracer, and you will keep working in the same repo in the next assignment. When making your design choices, think a bit further about what could be needed in the next assignment. As a hint, basically everything mentioned in lectures on lighting in ray tracing will be covered in the next assignment.

**The stencil is structured with three modules.**

- The utils module is the same thing you’ve seen in the parser lab. You should paste whatever you have in the parser lab into the project to make a functional parser.

- The camera module contains everything related to camera operation. You are required to implement part of the camera interface. Please refer to the comments for detailed instructions.

- The raytracer module is the main component of this assignment, although it only contains two classes, `RayTraceScene` and `RayTracer`. The class names kind of already give out what they are used for. In `RayTraceScene`, you are expected to construct the scene using the meta-data that you’ve built in the parser lab. In `RayTracer`, you will implement the ray tracing algorithm.

## 4 FAQ & Hints

### 4.1 Nothing shows up

Start with a simple scene, like the scene with only one primitive and make sure it’s working.

Make sure your math and numbers are making sense. 

### 4.2 How do I design my code so that my next assignment will not be a complete rewrite?

Don’t write everything in one function. Don’t write everything in one file. 

Try to keep your project modularized. Try to make your design in an object-oriented programming way.

### 4.3 My ray tracer runs extremely slow :(

Since the time complexity of the naive ray tracing algorithm is $O(MN)$, where M is the number of pixels and N is the number of primitives, it inevitably is going to be slow when the number of primitives in the scene gets large. 

But that’s not the end of the world and here are some tips for you at different stages.

**When you are still working on your project:**

Typically, modern CPUs should have enough horsepower to handle most of our scenes. Yet, you still definitely want to render your scene in a flash to check for bugs and errors. 

One thing you can do is to limit the resolution of your output image. Our default rendering resolution is $800\times800$, but you can reduce it by half or even more. And of course, you don’t want to start with any extremely complex scene. Here is the list of complex scenes that you should be aware of.

- List of complex scenes


Despite these scenes being complex, it does not mean that we are not going to test them on your ray tracer for grading. You are still expected to output reasonable results on these scenes in a reasonable amount of time.

**When you have finished the basic requirements and are looking for more:**

As covered in the lecture, ray tracing is ‘embarrassingly parallel’. Therefore, you can boost your performance simply by making your ray tracer parallel. And there are tons of ways to achieve this. Please refer to the extra credits section for more information.

It’s worth noting that acceleration data structure also offers **significant** performance boost. And there are plenty of choices for you.

If you are registered for the half-credit, it is strongly recommended that you do both for the requirement.

## 5 Extra Credits

* **Acceleration data structure**: There are various ways to build an acceleration data structure. Here are the approaches that are covered in the lecture.
    * Octree
    * BVH
    * KD-Tree
* **Parallelization**: There are many ways to parallelize your code. It can be as simple as one line of code in OpenMP. It can also go up to a carefully designed task scheduler that acts like Cinebench. Evidently, the grades you receive will be based on the complexity of your implementation. If you have more questions on the rubric and design choice, come to TA hours and we are happy to help.
    * OpenMP
    * Qt based implementation that has limited control of scheduling
    * Qt based implementation that has concrete control on scheduling
* **Anti-aliasing**: As we are only shooting one ray through the center of each pixel, it can be expected that there will be aliasing. In your filter project, you’ve learnt some ways to fix aliasing as a post-processing approach. Try to figure out if you can integrate your filter into your ray tracer as a post-processing module for your output image.
*  **Super-sampling**: As mentioned above, the source of aliasing is the low number of samples. So the most direct way to resolve it is to increase the number of samples. 
    * Naive super-sampling
    * Adaptive super-sampling

## 6 README

You must submit a README in Markdown format. This file should contain basic information about your design choices. You should also include any known bugs or any extra credits you’ve implemented. Please note that the README is worth **5 points** of your assignment grade!

## 7 Handling in

To submit your assignment, upload your repo to Gradescope via Github. For all assignments in this course, we will test your code on department machines, so we expect your code to be able to compile on department machines. If you are developing locally, please test your code before submitting. If your code does not compile, we will ask you to resubmit and this will result in a late penalty!
