# Puzzle
Research project ***from 2014*** of a mobile (Android) Augmented Reality application for the guidance of the assembly of a puzzle. 
In _Learning mode_ it automatically learn the parts and position of a given puzzle from images as shown on *img* folder. In the _Guidance mode_, show visual cues for assembly the puzzle.

The projects includes "directly" C++ code on Android app. The C++ development is on JNI folder and works for both **desktop** and **mobile**. For more information look at [NKD Guides](https://developer.android.com/ndk/guides).

The **desktop client** [main.cpp](/jni/main.cpp) learn the puzzle (parts and locations) from the path given in the first argument (if given), it opens a test image:

![Test puzzle image](/img/assets/cam_input_example.png)

Then it learn from the geometry of the parts and run a loop while the user needs guidance of the parts. The application don't track when the user puts the partg on the correct location, it only shows hitns of the parts position and the user has to manually click for the next hint.

The **mobile client** [src/com/example/puzzle](/src/com/example/puzzle) learn the parts from a path defined on [ar.cpp](/jni/ar.cpp). Then the runtime of the application is defined on [PuzzleActivity.java](/src/com/example/puzzle/PuzzleActivity.java) and the methods used from C code are declared on [Augmentation.java](/src/com/example/puzzle/Augmentation.java):
- [classify](#guidance-mode)
- [training](#learning-mode)
- next

## Notes
- The application uses OpenCv, thus is required to be installed on the device. [This](/bin/OpenCV_3.0.0_Manager_3.00_armeabi.apk) is the the apk that worked for me.  
- Use [Code::Blocks](https://www.codeblocks.org/) for C++ code. [Example of Code::Block configuration](/jni/CodeBlocksConf.cbp)
## Disclosure
The intention of this proyect was to integrate Computer Vision C++ code ***from 2014*** into a mobile device (In this case Android because it was easy to adquire) thus no further usability was intended. This proyect is discontinuated, is unknown if still works on new Android versions. See [why this is a naive implementation](#why-is-this-naive).

A puzzle is used for simulates the assembly of manufacturing projects and the use of IAR ([Industrial Augmented Reality](https://en.wikipedia.org/wiki/Industrial_augmented_reality)).

## Code Structure
Main code location
```
PuzzleApp
│── jni: Main code lives here
│   │── scr: Source code of vision modules are located under 
│   │── bin: Compiled desktop app 
│   │── main.cpp: Client scripts for desktop 
│   └── ar.cpp: Interface for export C++ classes for Android
│── src/com/example/puzzle: Native Android classes
│   │── Augmentation.java: Interface class that define the AR methods and loads from compiled C++
│   └── PuzzleActivity.java: Android activity of application
│── bin: Compiled Android app and OpenCV v3
└── img: Images of the Puzzle used for training 
```

# Vision Modes
The application have two main moments, in the first one, it learn the geometry of the parts and on the second one display hints for guide the assembly. This two modes are integrated on the same function ```ArMoments::calcMoment```

## Learning Mode
The learning mode is excecuted at the beginning of the runtime. It uses the images located on a folder with a specific structure as shown [here](/img) and contains:

- parts: each part defined as a folder that is numerated starting from 1 inside images of the part

![Part Example](/img/assets/part_2_example.png)

_Example training image part 2_

- assembly: for each folder how does each step of the assembly looks

![Part Example](/img/assets/assembly_step_3.png)

_Example assembly step 3_

The learning mode uses a 7 dimension vector with the [Hu moments](https://en.wikipedia.org/wiki/Image_moment) as descriptor of each part (known as their features). The Hu moments are commonly used in classic vision problems due to their invariability to Translation, Rotation and Scale (TRS) transformations meaning the position, rotation and distance of the camera will not affect (very small changes) the value of the descriptor vector of each part.

Then a simplified [K-nearest neighbors algorithm](https://en.wikipedia.org/wiki/K-nearest_neighbors_algorithm) (KNN) located on [KNN.cpp](/jni/src/KNN.cpp) is "trained". As the descriptors are invariant to TRS the training is just and addition of each vector to a matrix where each row is the index of the part and the columns the moments.

# The Puzzle
The puzzle is commontly used on research to study de assembly of products in a controlled way. The proposed puzzle has the characteristics that each part and all mid the assembly steps have a completly different geometry. 
Additionally all the configurations looks different to be able to identify 

## Guidance Mode
After the learning phase is performed each new image is passed to the function ```ArMoments::calcMoment``` where some kind of segmentation is performed to [find the contours](https://docs.opencv.org/3.4/df/d0d/tutorial_find_contours.html) on the image. For each contour found a vector with the Hu moments is calculated and calculated its near neighbour on the matrix builded on training. 


# Why is this naive?
This is part of a research project thus its purpose is only to be able to use a mobile device. Some of the reasons this could not be used in any other real-life application:

- Segmentation is poorly done, only works on clear background with black parts (It is a research project)
- Using Hu moments as features: they are not the most robust regarding affine transformations. Also small or similar parts will fail. If you are doing something similar use deep learning
- AR guidance is poor
- Only works on flat parts
- Is highly influenced by lightng as shadows can get taken as geometry of the parts