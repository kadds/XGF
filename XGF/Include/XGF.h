#pragma once
//所有Initialize(初始化) 和 Shutdown (结束) 函数需手动调用


//*********** Core ***************
#include "GDI.hpp"
#include "XGFramework.hpp"
#include "Shader.hpp"
#include "Batch.hpp"
#include "Camera.hpp"
#include "PerspectiveCamera.hpp"
#include "OrthoCamera.hpp"
#include "FPSCamera.hpp"
#include "Font.hpp"
#include "TextRenderer.hpp"
#include "Scene.hpp"
#include "ShapeRenderer.hpp"
#include "ShaderManager.hpp"
#include "ResourceManager.hpp"
#include "RenderToTexture.hpp"
#include "Context.hpp"
#include "Renderer.hpp"
//********************************

//*********** UI *******************
#include "Label.hpp"
#include "Button.hpp"
#include "EditText.hpp"
//**********************************

//****** Animation And Interpolator *********
#include "Interpolator\Interpolator.hpp"
#include "Action.hpp"
#include "Animation.hpp"
//***********************************

//************ Tools ******************
#include "DX8Input.hpp"
#include "Application.hpp"
#include "Tools.hpp"
#include "DebugInscriber.hpp"
#include "AsyncTask.hpp"
#include "AxisRenderer.hpp"
#include "Defines.hpp"
#include "ScreenGrab.h"
#include "GridRenderer.hpp"
//***********************************

//************** Shape ****************
#include "Shape.hpp"
#include "Rectangle.hpp"
#include "RectangleB.hpp"
#include "RectangleC.hpp"
#include "Line.hpp"
#include "Circle.hpp"
#include "Mesh.hpp"
#include "MeshRenderer.hpp"
#include "Geometry\BoxGeometry.hpp"
#include "Geometry\PlaneGeometry.hpp"
#include "Geometry/SphereGeometry.hpp"
#include "Material\BasicMaterial.hpp"
#include "Material/PhongMaterial.hpp"
#include "Material/LambertMaterial.hpp"
#include "Light/AmbientLight.hpp"
#include "Light/DirectionalLight.hpp"
#include "Light/PointLight.hpp"
#include "Light/SpotLight.hpp"

//***********************************
#include "TrackballCameraController.hpp"