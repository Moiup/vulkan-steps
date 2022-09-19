# **Vulkan Steps**

## **Introduction**

Vulkan can be long to learn, and getting a first outcome can be very slow. The purpose here is not to make a propper tutorial, but more to provide the different steps to build a basic Vulkan application. In fact, there are multiple goals. The first one, is to have a quick access to the code required for a given part, but also teaching Vulkan the hard way : through the [specification](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/index.html).

Unfortunately, it seems that if we are not provided at least with the steps required to build a renderer, it is kinda hard (if not impossible) to know what to look for in the [specification](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/index.html). Thus, the table of content that I provide is very important as it is the first step towards learning Vulkan. Indeed, personnaly to learn Vulkan, I looked at the table of content of the tutorial from [LunarG](https://vulkan.lunarg.com/doc/view/1.2.154.1/windows/tutorial/html/index.html) and tried, only by looking at the title, to find the corresponding part into the [specification](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/index.html) and try to implement it without looking. It was an exercise for me to force myself into looking at the [specification](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/index.html) and the using the content of the tutorial as a correction.

Each step of the tutorial, but the 0, will be composed of three parts:
 - Its title which refer directly to a part of the [specification](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/index.html).
 - The final function called in this step. Most of the steps are build following the same scheme: some structures to fill before passing them to a function. Thus, by having the name of the final function, or some functions if the step is too tricky, it is possible to deduce the structures to create and the other functions to call by reading the [specification](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/index.html).
 - The code of the step.

 I like to see this approach as multiplie exercices. The goal of each exercise is expressed by the title. Then, if stuck, some hints are given to tell what to look for in the [specification](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/index.html). Finally, the correction of the exercise (the code).

If it is not already done, [click here to open the Vulkan specification](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/index.html).

The main source of information that helped build a first Vulkan application where:
 - [LunarG](https://vulkan.lunarg.com/doc/view/1.2.154.1/windows/tutorial/html/index.html)
 - []()
 - The [specification](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/index.html), of course.

### **Disclaimer**
This is not a tutorial to teach rendering, having a background in others graphic API such as OpenGL is almost mendatory.

The code that I provide is not the cleanest, and this is done on purpose. Why? Because I did not wanted to build functions so that the reader can follow the tutorial linearly (copy / paste every step one after the other). Of course, the reader is encourage to build his own functions, classes and whatever he wants.

## **Table of contents**