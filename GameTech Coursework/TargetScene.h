#pragma once

#include <nclgl\NCLDebug.h>
#include <ncltech\Scene.h>
#include <ncltech\SceneManager.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\DistanceConstraint.h>
#include <ncltech\CommonUtils.h>

//Fully striped back scene to use as a template for new scenes.
class TargetScene : public Scene
{
public:
	TargetScene(const std::string& friendly_name)
		: Scene(friendly_name)
	{
	}

	virtual ~TargetScene()
	{
	}


	float m_RampAngle;

	const Vector3 floor_pos = Vector3(0, 1.5f, 0);

	virtual void OnInitializeScene() override
	{

		PhysicsEngine::Instance()->SetGravity(Vector3(0.0f, -10.0f, 0.0f));		//No Gravity!
		PhysicsEngine::Instance()->SetDampingFactor(1.0f);						//No Damping!

		for (int i = 0; i < objects.size(); i++) {
			objects[i].clear();
		}
		objects.clear();

		{
			// create targets
			this->AddGameObject(CommonUtils::BuildCuboidObject("c1",
				floor_pos,	//Position leading to 0.25 meter overlap on faces, and more on diagonals
				Vector3(5.0f, 0.2f, 7.0f),				//Half dimensions
				true,									//Has Physics Object
				0.0f,									//Infinite Mass
				true,									//Has Collision Shape
				false,									//Dragable by the user
				CommonUtils::GenColor(0.3f, 0.5f)));	//Color

			this->AddGameObject(CommonUtils::BuildCuboidObject("c2",
				floor_pos + Vector3(0, 5, -7),								//Position
				Vector3(2.0f, 0.2f, 2.0f),				//Half dimensions
				true,									//Has Physics Object
				0.0f,									//Infinite Mass
				true,									//Has Collision Shape
				false,									//Dragable by the user
				CommonUtils::GenColor(0.7f, 0.5f)));	//Color
			GameObject* c2 = this->FindGameObject("c2");
			c2->Physics()->SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(1.0f, 0.0f, 0.0f), 90.0f));

			//this->AddGameObject(CommonUtils::BuildCuboidObject("c3",
			//	floor_pos + Vector3(0, 5, -4.5),								//Position
			//	Vector3(1.0f, 0.1f, 1.0f),				//Half dimensions
			//	true,									//Has Physics Object
			//	0.0f,									//Infinite Mass
			//	true,									//Has Collision Shape
			//	false,									//Dragable by the user
			//	CommonUtils::GenColor(0.7f, 0.5f)));	//Color
			//GameObject* c3 = this->FindGameObject("c3");
			//c3->Physics()->SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(1.0f, 0.0f, 0.0f), 90.0f));

			//this->AddGameObject(CommonUtils::BuildCuboidObject("c4",
			//	floor_pos + Vector3(-1.15, 5, -5.5),								//Position
			//	Vector3(1.0f, 0.1f, 1.0f),				//Half dimensions
			//	true,									//Has Physics Object
			//	0.0f,									//Infinite Mass
			//	true,									//Has Collision Shape
			//	false,									//Dragable by the user
			//	CommonUtils::GenColor(0.7f, 0.5f)));	//Color
			//GameObject* c4 = this->FindGameObject("c4");
			//c4->Physics()->SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0.0f, 0.0f, 1.0f), 90.0f));

			//this->AddGameObject(CommonUtils::BuildCuboidObject("c5",
			//	floor_pos + Vector3(1.15, 5, -5.5),								//Position
			//	Vector3(1.0f, 0.1f, 1.0f),				//Half dimensions
			//	true,									//Has Physics Object
			//	0.0f,									//Infinite Mass
			//	true,									//Has Collision Shape
			//	false,									//Dragable by the user
			//	CommonUtils::GenColor(0.7f, 0.5f)));	//Color
			//GameObject* c5 = this->FindGameObject("c5");
			//c5->Physics()->SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0.0f, 0.0f, 1.0f), 90.0f));

		}


		const int dims = 30;
		const int dimsx = dims ;
		const int dimsz = dims  * 0.25f;
		float parts = 360.0f / (float)dimsx / 4.5f;
		auto create_ball_cloth = [&](const Vector3& offset, const Vector3& scale, float ballsize)
		{

			float m;
			for (int x = 0; x < dimsx; ++x)
			{
				float rad = DegToRad(x * parts);
				objects.push_back(std::vector<GameObject*>());
				for (int z = 0; z < dimsz; ++z)
				{
					if (z == 0)  m = 0.0f;
					else m = 150.0f;
					Vector4 col = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
					//	Vector3 pos = offset + Vector3(scale.x * x * 0.5, 0.0f, scale.z * z * 0.5);
					Vector3 pos = offset + Vector3(cos(rad*4.5f), -scale.y * z * 0.5, sin(rad*4.5f));
					GameObject* sphere = CommonUtils::BuildNonRenderObject(
						"",					// Optional: Name
						pos,				// Position
						ballsize,			// Half-Dimensions
						true,				// Physics Enabled?
						m,				// Physical Mass (must have physics enabled)
						true,				// Physically Collidable (has collision shape)
						false,				// Dragable by user?
						col);// Render color
					this->AddGameObject(sphere);
					objects[x].push_back(sphere);

				}
			}
		};
		create_ball_cloth(floor_pos + Vector3(0, 5, -5), Vector3(0.5f, 0.5f, 0.5f), 0.04f);

		for (int i = 0; i < dimsx; i++) {
			for (int j = 0; j < dimsz; j++) {
				int indexi = i - 1;
				for (int c = 0; c < 3; c++) {
					int indexj = j - 1 + c;
					if (indexi < dimsx && indexi > -1 && indexj < dimsz && indexj > -1) {
						DistanceConstraint* constraint = new DistanceConstraint(
							objects[i][j]->Physics(),					//Physics Object A
							objects[indexi][indexj]->Physics(),					//Physics Object B
							objects[i][j]->Physics()->GetPosition(),	//Attachment Position on Object A	-> Currently the centre
							objects[indexi][indexj]->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  
						PhysicsEngine::Instance()->AddConstraint(constraint);
					}

				}
				indexi = i;
				for (int c = 0; c < 3; c += 2) {
					int indexj = j - 1 + c;
					if (indexi < dimsx && indexi > -1 && indexj < dimsz && indexj > -1) {
						DistanceConstraint* constraint = new DistanceConstraint(
							objects[i][j]->Physics(),					//Physics Object A
							objects[indexi][indexj]->Physics(),					//Physics Object B
							objects[i][j]->Physics()->GetPosition(),	//Attachment Position on Object A	-> Currently the centre
							objects[indexi][indexj]->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  
						PhysicsEngine::Instance()->AddConstraint(constraint);
					}
				}
				indexi = i + 1;
				for (int c = 0; c < 3; c++) {
					int indexj = j - 1 + c;
					if (indexi < dimsx && indexi > -1 && indexj < dimsz && indexj > -1) {
						DistanceConstraint* constraint = new DistanceConstraint(
							objects[i][j]->Physics(),					//Physics Object A
							objects[indexi][indexj]->Physics(),					//Physics Object B
							objects[i][j]->Physics()->GetPosition(),	//Attachment Position on Object A	-> Currently the centre
							objects[indexi][indexj]->Physics()->GetPosition());	//Attachment Position on Object B	-> Currently the centre  
						PhysicsEngine::Instance()->AddConstraint(constraint);
					}
				}
			}
		}

	}

	void draw() {

		for (int i = 0; i < objects.size(); i++) {
			for (int j = 0; j < objects[i].size() - 1; j++) {
				NCLDebug::DrawThickLine(objects[i][j]->Physics()->GetPosition(), objects[i][j + 1]->Physics()->GetPosition(), 0.05f, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			}
		}

		for (int i = 0; i < objects.size() - 1; i++) {
			for (int j = 0; j < objects[i].size(); j++) {
				if (j == 0)  NCLDebug::DrawThickLine(objects[i][j]->Physics()->GetPosition(), objects[i + 1][j]->Physics()->GetPosition(), 0.15f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
				else NCLDebug::DrawThickLine(objects[i][j]->Physics()->GetPosition(), objects[i + 1][j]->Physics()->GetPosition(), 0.05f, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			}
		}

		for (int j = 0; j < objects[0].size(); j++) {
			if (j == 0)  NCLDebug::DrawThickLine(objects[objects.size() - 1][j]->Physics()->GetPosition(), objects[0][j]->Physics()->GetPosition(), 0.05f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
			else NCLDebug::DrawThickLine(objects[objects.size() - 1][j]->Physics()->GetPosition(), objects[0][j]->Physics()->GetPosition(), 0.05f, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		}

	}

	


	void spawn() {

		//Create a projectile
		Camera * camera = GraphicsPipeline::Instance()->GetCamera();
		float yaw = camera->GetYaw();
		float pitch = camera->GetPitch();
		float fx, fz, fy;
		fx = -sin(DegToRad(yaw));
		fz = -cos(DegToRad(yaw));
		//fy = sin(DegToRad(pitch));
		//float absp = 1 - (abs(fy));

		Vector4 col = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		GameObject* sphere = CommonUtils::BuildSphereObject(
			"",					// Optional: Name
			camera->GetPosition(),				// Position
			0.5f,			// Half-Dimensions
			true,				// Physics Enabled?
			1.0f,				// Physical Mass (must have physics enabled)
			true,				// Physically Collidable (has collision shape)
			false,				// Dragable by user?
			col);// Render color
		sphere->Physics()->SetLinearVelocity((Vector3(fx * 15, 2 + pitch * 0.5, fz * 15)));
		sphere->Physics()->SetAngularVelocity((Vector3(-5, 0, 0)));
		this->AddGameObject(sphere);
	}
	

	float m_AccumTime;
	virtual void OnUpdateScene(float dt) override
	{
		Scene::OnUpdateScene(dt);
		draw();

		//Update Rotating Objects!
		if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_J))
		{
			spawn();
		}


	}
	private:
		std::vector<std::vector<GameObject*>> objects;

};