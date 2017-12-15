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
#include "ShaderConst.hpp"
#include "ShapeRenderer.hpp"
#include "RenderToTexture.hpp"
#include "ParticleSystem.hpp"
#include "ConstantData.hpp"
//********************************

//*********** UI *******************
#include "Layer.hpp"
#include "UILayer.hpp"
#include "Label.hpp"
#include "Button.hpp"
#include "EditText.hpp"
#include "UIBatches.hpp"
//**********************************

//****** Animation And Interpolator *********
#include "Interpolator\Interpolator.hpp"
#include "ActionBuilder.hpp"
#include "Action.hpp"
#include "SequenceAction.hpp"
#include "ParallelAction.hpp"
#include "ChangeColorAction.hpp"
#include "RotateToAction.hpp"
#include "ScaleToAction.hpp"
#include "MoveToAction.hpp"
#include "Animation.hpp"
//***********************************

//************ Tools ******************
#include "DX8Input.hpp"
#include "InputListener.hpp"
#include "Application.hpp"
#include "Tools.hpp"
#include "DebugInscriber.hpp"
#include "FPS.hpp"
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
#include "Cube.hpp"
#include "Circle.hpp"
//***********************************