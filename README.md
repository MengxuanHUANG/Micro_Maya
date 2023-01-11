# Micro_Maya
This is an individual assignment in CIS5600 at the University of Pennsylvania. In this project, I applied half-edge model for mesh loading and efficiently mesh editing. Besides, the application supports to load skeleton as well as automatically skinning.

## Featues Demonstration
* [Camera Operation](https://github.com/MengxuanHUANG/Micro_Maya#polar-spherical-camera-operation)
* [Component Selection](https://github.com/MengxuanHUANG/Micro_Maya#selection)
* [Split Edge](https://github.com/MengxuanHUANG/Micro_Maya#split-edge)
* [Insert EdgeLoop](https://github.com/MengxuanHUANG/Micro_Maya#insert-edgeloop)
* [Triangulate Face](https://github.com/MengxuanHUANG/Micro_Maya#triangulate-face)
* [Extrude Face](https://github.com/MengxuanHUANG/Micro_Maya#extrude-face)
* [Bevel](https://github.com/MengxuanHUANG/Micro_Maya#bevel)
* [Mesh Subdivision](https://github.com/MengxuanHUANG/Micro_Maya#mesh-subdivision)
* [Auto Skinning](https://github.com/MengxuanHUANG/Micro_Maya#auto-skinning)
* [Mannually Adujst Joints' Weight](https://github.com/MengxuanHUANG/Micro_Maya#mannually-adujst-joints-weight)

### Mesh Loading & Mesh editing
#### Polar Spherical Camera Operation  
`Arrow UP/DOWN/LEFT/RIGHT` moves along the surface of a sphere  
`W` zoom in  
`S` zoom out  
`A` pan left reference point  
`D` pan right reference point  
`Q` pan up reference point  
`E` pan down reference point  
`R` reset camera position and orientation  

#### Selection
1. Click `Vertex`, `Edge`, `Face` to select different items. Besides, it also supports to press `shift` for multi-selection and deselection.

![image](https://user-images.githubusercontent.com/53021701/211701569-7e5ceb72-a0bf-40c2-8936-a7d1bb06f3ba.png)
![image](https://user-images.githubusercontent.com/53021701/211701643-c3daa820-20d5-46a0-8c65-ed76e8d31476.png)
![image](https://user-images.githubusercontent.com/53021701/211701686-5d99afe9-db5a-415d-8d77-488c4c44c3a9.png)

It supports to select different items at the same time.
![image](https://user-images.githubusercontent.com/53021701/211701774-858b3496-49bc-48d0-8e97-fd7275759de8.png)

Press `Shift` and double click `Mouse Left Button` to select edge loop
![image](https://user-images.githubusercontent.com/53021701/211702390-55cd84b2-045f-4a0f-b869-769219814c87.png)

2. Click items in the lists  
Use `Shift` and `Alt` for multi-selection and deselection.
![image](https://user-images.githubusercontent.com/53021701/211701833-021c11f2-3088-492e-9b83-34c86641d061.png)

#### Split Edge    
Select an edge or multiply edges and press `Add Vertex` button to split edges.
![image](https://user-images.githubusercontent.com/53021701/211702867-ff128ebe-6c04-4f38-bf99-cb84330a1588.png)
![image](https://user-images.githubusercontent.com/53021701/211702903-6297ddf2-167c-4fbf-a520-56125659f553.png)

#### Insert EdgeLoop  
Select a parallel edges-loop, then, press the `Insert EdgeLoop` button to insert an edge loop.
![image](https://user-images.githubusercontent.com/53021701/211703125-93f1ea18-6a7c-40ca-a450-846701e378f6.png)
![image](https://user-images.githubusercontent.com/53021701/211703217-bfb97a69-ab92-45f3-866f-639e4f0db776.png)

#### Triangulate Face  
Select a face or multiply faces and press `Triangulate Face` button.
![image](https://user-images.githubusercontent.com/53021701/211703464-ef162b91-c611-4a5d-ba7e-79e7cff6602a.png)
![image](https://user-images.githubusercontent.com/53021701/211703497-040526a8-6293-4154-900c-cb72f8615535.png)

#### Extrude Face  
Select a face or multiply faces and press `Extrude Face` button.
![image](https://user-images.githubusercontent.com/53021701/211703688-22411b04-828c-414f-ac30-0c75347a0afa.png)
![image](https://user-images.githubusercontent.com/53021701/211703712-c3957edc-151b-4f95-9947-9011593f2762.png)

#### Bevel  
Select faces and press the `Bevel` button.  
Default configuration:  
segment: 4  
width: 0.5  
depth: 1.0  
![image](https://user-images.githubusercontent.com/53021701/211703841-3faba856-9110-4048-b9eb-ed2078d1e254.png)
![image](https://user-images.githubusercontent.com/53021701/211703864-693abc08-852e-4a6c-92bd-db49db282e51.png)

#### Mesh Subdivision  
Press `Subdivision` button to subdivide mesh once
![image](https://user-images.githubusercontent.com/53021701/211704086-8d2edefb-2c6f-41c7-b093-8d8e07ddb50a.png)
1<sup>st</sup> Subdivision
![image](https://user-images.githubusercontent.com/53021701/211704121-d1ac2783-49dc-4efe-9148-41628af56d06.png)
2<sup>nd</sup> Subdivision
![image](https://user-images.githubusercontent.com/53021701/211704325-3117b157-2b9a-4be7-b693-0f0949d0f56e.png)
3<sup>rd</sup> Subdivision
![image](https://user-images.githubusercontent.com/53021701/211704359-480b2e26-8b46-4730-a6df-1debb967308f.png)

Before subdivision, `Vertex`, `Edge`, `Face` can be marked as `Sharp`
![image](https://user-images.githubusercontent.com/53021701/211704665-e96dd3be-10d0-41d6-8b5f-28bfd48ddf54.png)
1<sup>st</sup> Subdivision
![image](https://user-images.githubusercontent.com/53021701/211704734-ae078e32-71fb-4a98-86b3-6eedca98a382.png)
2<sup>nd</sup> Subdivision
![image](https://user-images.githubusercontent.com/53021701/211704826-68f4a384-b3a8-41aa-bdd3-2280cfe7bec6.png)
3<sup>rd</sup> Subdivision
![image](https://user-images.githubusercontent.com/53021701/211704961-6a3d3b6a-f589-4a42-9f41-f0d9790404b2.png)

### Skeleton & Skinning  
Firstly, load a mesh and a skeleton. 
#### Auto skinning  
Press the `Skinning` button to automatically make a skeleton mesh
![image](https://user-images.githubusercontent.com/53021701/211705453-1cb8535e-74b7-4db9-af47-484495b27726.png)
![image](https://user-images.githubusercontent.com/53021701/211705480-2177d2da-d8b6-4ca4-8aa5-921486d57f97.png)

Use `Mouse Left Button` to select a joint and view its influence on the mesh
![image](https://user-images.githubusercontent.com/53021701/211705615-915e9c4c-749b-4a5b-b703-190ee93e60b4.png)

#### Mannually adujst joints' weight  
Select a vertex. Then, use the panel on the right to modify joints influence on the select vertex.
![image](https://user-images.githubusercontent.com/53021701/211705828-89db718a-2be0-41f3-8b1a-85aa2213ded6.png)
