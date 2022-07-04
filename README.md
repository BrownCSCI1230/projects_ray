# Projects 3 & 4: Ray

In the previous two projects, you are building separate functionalities. However, starting from this project, you will start to build something bigger and the tasks will be divided into two sub-project, each taking two weeks long. 

## 1. Introduction

In the next two assignments, you will implement a **ray tracer**, which supports various lighting effects. As a teaser, here are some results that your final ray tracer is expected to be able to produce.

**The overview of ray tracing pipeline**

In lecture, you've learned that the ray tracing pipeline can be roughly divided into these three components. 

In Ray 1, you will mainly focus on the first two parts of the ray tracing algorithm

- Generating and casting rays
- Finding the closest intersection point
  - Implicit shape primitives
  - Intersection pipeline

In Ray 2, you will extend your own implementation and make a completely functional ray tracer, the tasks you will be doing are roughly listed as follow.

- Computing the lighting at the intersection point
  - Phong lighting model
  - Different light sources
  - Texture mapping
  - Recursive ray tracing

## 2. Project Stencil

Unlike the previous two assignments, you will use command line to interact with the ray tracer, take a look at `main.cpp` and check how you are supposed to pass in parameters as the render configuration.

You may have noticed that there is only a limited amount of stencil code is provided for this assignment. Hence, it means that it's up to you to make any design choice for you project. You have all the freedom to add any additional file that you think is necessary for your ray tracer. However, **do not** modify any of the existing interface, as we are relying on those to auto-grade your project!

For specific coding advice, please refer to the handout of the specific project.

