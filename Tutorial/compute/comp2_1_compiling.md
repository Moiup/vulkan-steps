# **Comp 2.1: Compiling and Reading shader**

## **Writing shader**
The shader used in our example. It takes an array as input. It computes the sum between the value stored in the input array and the value of the thread id. The final result is stored in an output array.

## Compute Shader
```c
#version 450

layout (std430, binding = 0) readonly buffer inputData
{
    int input_data[];
};

layout (std430, binding = 1) writeonly buffer outputData
{
    int output_data[];
};

layout(local_size_x = 8) in;
void main(){
    const uint id = gl_GlobalInvocationID.x;
    const uint data_length = input_data.length();

    if(id > data_length){
        return;
    }

    output_data[id] = input_data[id] + int(id);
}
```

## **Compiling**
To compile, we use `glslc` that we found in the VulkanSDK folder, inside `Bin`. For windows users: `glslc.exe`

To compile, open up the command line, go to the folder containing your compute shader, and run:
```
C:/VulkanSDK/version_of_Vulkan/Bin/glslc.exe compute.comp -o compute.spv
```

| Previous | Next |
|---|---|
| [Comp 2: Compute Shader](comp2_compute_shader.md) | [Comp 2.2: Shader Module](comp2_2_shader_module.md) |