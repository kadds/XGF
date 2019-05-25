#pragma once
// All Initialize and Shutdown functions need to be called manually


//*********** Core ***************
#include "GDI.hpp"
#include "XGFramework.hpp"
#include "Shader.hpp"
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
#include "Context.hpp"
#include "Renderer.hpp"
#include "SystemShaders.hpp"
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
#include "Material/PhysicsMaterial.hpp"
#include "Light/AmbientLight.hpp"
#include "Light/DirectionalLight.hpp"
#include "Light/PointLight.hpp"
#include "Light/SpotLight.hpp"

//***********************************
#include "TrackballCameraController.hpp"