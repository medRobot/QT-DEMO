cmake_minimum_required(VERSION 2.8.11)

PROJECT(imageProcess)

SET(QT_USE_QTSQL TRUE)

FIND_PACKAGE (QT4 REQUIRED)
INCLUDE (${QT_USE_FILE})
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_BINARY_DIR})
ADD_DEFINITIONS (${QT_DEFINITIONS})

FIND_PACKAGE(OpenCV REQUIRED )
INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIR})

FIND_PACKAGE(ITK)
INCLUDE(${ITK_USE_FILE})

FIND_PACKAGE(VTK)
INCLUDE(${VTK_USE_FILE})

SET (imageProcess_HEADERS  
geometryTransform.h
itkFilter.h
morphology.h
pointOperation.h
visualization.h
registration.h
orthogonalTransform.h
edge_contour.h
segmentation.h
vtkCustomCommands.h
dicomProcess.h
mainwindow.h 
mainwindowInterator.h 
)

SET(imageProcess_RESOURCES imageprocess.qrc)

SET (imageProcess_UIS 
imageProcess.ui 
edge_contour.ui 
itkFilter.ui 
morphology.ui
orthogonalTransform.ui
pointOperation.ui
visualization.ui
registration.ui
segmentation.ui
geometryTransform.ui
dicomProcess.ui
)

SET (imageProcess_SRCS  

geometryTransform.h
itkFilter.h
morphology.h
pointOperation.h
visualization.h
registration.h
orthogonalTransform.h
edge_contour.h
segmentation.h
vtkCustomCommands.h
dicomProcess.h
mainwindow.h 
mainwindowInterator.h 

geometryTransform.cpp
itkFilter.cpp
morphology.cpp
pointOperation.cpp
visualization.cpp
registration.cpp
orthogonalTransform.cpp
edge_contour.cpp
segmentation.cpp
vtkCustomCommands.cpp
dicomProcess.cpp
main.cpp 
mainwindow.cpp     
mainwindowInterator.cpp

imageprocess.qrc                  
imageProcess.rc
)

QT4_WRAP_CPP      (imageProcess_HEADERS_MOC ${imageProcess_HEADERS})

QT4_ADD_RESOURCES (imageProcess_RESOURCES_RCC ${imageProcess_RESOURCES})

QT4_WRAP_UI       (imageProcess_UIS_UIC     ${imageProcess_UIS})


ADD_EXECUTABLE (imageProcess ${imageProcess_SRCS}
                            
                            ${imageProcess_HEADERS_MOC}

                            ${imageProcess_RESOURCES_RCC}

                            ${imageProcess_UIS_UIC})


TARGET_LINK_LIBRARIES (imageProcess ${QT_LIBRARIES})

TARGET_LINK_LIBRARIES(imageProcess ${OpenCV_LIBS})

TARGET_LINK_LIBRARIES(imageProcess ${ITK_LIBRARIES})

TARGET_LINK_LIBRARIES(imageProcess ${VTK_LIBRARIES} QVTK)
